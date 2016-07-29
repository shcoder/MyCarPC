using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace UploaderUI {
    public enum Level {
        Error, Warning, Info
    }

    public enum CommondCommandType {
        E_CC_NOOP = 0,
        E_CC_WRITE = 1,
        E_CC_READ = 2,
        E_CC_WRITE_AFTER_WREN = 3,
        E_CC_WRITE_AFTER_EWSR = 4,
        E_CC_ERASE = 5
    }

    class CommuticationCanceledException : Exception {
        public CommuticationCanceledException(System.IO.IOException IOException) {
            this.IOException = IOException;
        }
        public System.IO.IOException IOException { get; protected set; }
    }

    public partial class Communication {
        MainWindow window;
        SerialPort com;

        public Communication(MainWindow window) {
            this.window = window;

            System.Threading.Tasks.Task.Run((Action)ReadThread);
            System.Threading.Tasks.Task.Run((Action)UpdateStatus);
            System.Threading.Tasks.Task.Run((Action)CommandProcessThread);
        }

        public void Log(string message, Level level, params object[] args) {
            window.Log(message, level, args);
        }

        public void Open(string port) {
            if (com != null && com.IsOpen)
                Close();
            com = new SerialPort(port, 115200, Parity.None, 8, StopBits.One);
            com.RtsEnable = true;
            com.Handshake = Handshake.None;
            //com.DataReceived += Com_DataReceived;
            com.Open();
            if (com.IsOpen)
                Log("COM port is opened", Level.Info);
            else
                Log("Fail open COM port", Level.Error);
            com.ReceivedBytesThreshold = 1;
        }

        public void Close() {
            Task.Run(() => {
                try {
                    com.Close();
                } catch { }
            });
        }

        protected void UpdateStatus() {
            try {
                while (true) {
                    string status = "unknown";
                    if (com == null)
                        status = "not created";
                    else
                        status = com.IsOpen ? "opened" : "closed";

                    window.Dispatcher.Invoke(() => {
                        window.lbStatus.Content = status;
                        window.cbCOMPorts.IsEnabled = status != "opened";
                        window.buttonOpen.IsEnabled = status != "opened";
                        window.buttonClose.IsEnabled = status == "opened";
                    });


                    System.Threading.Thread.Sleep(5);
                }
            } catch (System.Threading.Tasks.TaskCanceledException) {
            } catch (Exception e) {
                Log("Unhandled exception in UpdateStatus: {0} - {1} in {2}", Level.Error, e.GetType().FullName, e.Message, e.StackTrace);
            }
        }

        public byte COMReadByte(bool calcCheckSum = true) {
            try {
                byte b = (byte)com.ReadByte();
                if (calcCheckSum)
                    checkSum = (byte)((int)checkSum ^ b);
                return b;
            } catch (System.IO.IOException e) {
                throw new CommuticationCanceledException(e);
            }
        }

        public ushort COMReadUShort() {
            return (ushort)((ushort)(COMReadByte() << 8) | (ushort)COMReadByte());
        }

        public char COMReadChar() {
            return (char)COMReadByte();
        }

        public string COMReadToChar(char stop) {
            return ReadToDelemiter(stop, -1);
        }

        public string ReadToDelemiter(char stop, int maxChars) {
            string res = "";
            while (true) {
                char c = COMReadChar();

                if (c == stop)
                    return res;
                res += c;
                if (maxChars > 0 && res.Length >= maxChars)
                    return res;
            }
        }

        byte checkSum = 0;
        Queue<Command> commandsQueue = new Queue<Command>();

        protected void ReadThread() {
            try {
                while (true) {
                    if (com == null || !com.IsOpen) {
                        Thread.Sleep(5);
                        continue;
                    }
                    var c = COMReadChar();
                    if (c != '$') // ждём начала команды
                        continue;
                    checkSum = 0;

                    c = COMReadChar(); //читаем команду
                    Command cmd;
                    switch (c) {
                        case 'V': // ответ на команду версии
                            cmd = new VersionCommand(this);
                            if (!cmd.Read()) {
                                Log("Ошибка разбора команды V", Level.Warning);
                                continue;
                            }
                            break;
                        case 'M': // сообщение от Arduino
                            cmd = new MessageCommand(this);
                            if (!cmd.Read()) {
                                Log("Ошибка разбора команды M", Level.Warning);
                                continue;
                            }
                            break;
                        case 'R':
                            cmd = new ReadRegResultCommand(this);
                            if (!cmd.Read()) {
                                Log("Ошибка разбора команды R", Level.Warning);
                                continue;
                            }
                            break;
                        case 'B':
                            cmd = new ReadBytesResultCommand(this);
                            if (!cmd.Read()) {
                                Log("Ошибка разбора команды B", Level.Warning);
                                continue;
                            }
                            break;
                        default:
                            Log("Неизвестная команда: {0}", Level.Info, c);
                            continue;
                    }

                    // проверяем хвост и контрольную сумму
                    char fin = COMReadChar(); // метка хвоста или контрольной суммы
                    if (fin == '@') { // пришёл маркер контрольной суммы
                        byte readedCheckSum = COMReadByte(false);
                        if (readedCheckSum != checkSum) {
                            Log("ReadThread: не коректная контрольная сумма у команды ${0}. Ожидается {1}, а был получен {2}",
                                Level.Error, c, checkSum, readedCheckSum);
                            continue;
                        }
                        fin = COMReadChar(); // снова читаем маркер хвоста
                    }
                    if (fin != '#') {
                        Log("ReadThread: не коректный маркер конца сообщения ${0}. Ожидается #, а был получен {1}",
                            Level.Error, c, fin);
                        continue;
                    }

                    // Добавляем валидную команду в очередь на выполнение
                    commandsQueue.Enqueue(cmd);

                    System.Threading.Thread.Sleep(5);
                }
            } catch (CommuticationCanceledException) {
            } catch (System.Threading.Tasks.TaskCanceledException) {
            } catch (Exception e) {
                Log("Unhandled exception in ReadThread: {0} - {1} in {2}", Level.Error, e.GetType().FullName, e.Message, e.StackTrace);
            }
        }

        protected void CommandProcessThread() {
            try {
                while (true) {
                    Command cmd;
                    try {
                        cmd = commandsQueue.Dequeue();
                    } catch (InvalidOperationException) { continue; }

                    cmd.Run();
                }
            //} catch (CommuticationCanceledException) {
            } catch (System.Threading.Tasks.TaskCanceledException) {
            } catch (Exception e) {
                Log("Unhandled exception in CommandProcessThread: {0} - {1} in {2}", Level.Error, e.GetType().FullName, e.Message, e.StackTrace);
            }
        }

        public void SetI2CAddress(byte addr) {
            byte[] buf = new byte[6];
            buf[0] = (byte)'$';
            buf[1] = (byte)'A';
            buf[2] = addr;
            buf[3] = (byte)'@';
            //buf[4] = ;
            buf[5] = (byte)'#';

            byte cs = 0;
            for (int i = 1; i <= 3; i++)
                cs = (byte)((int)cs ^ buf[i]);
            buf[4] = cs;
            com.Write(buf, 0, buf.Length);
        }

        public void Write(string s) {
            com.Write(s);
        }

        public byte ReadReg(byte reg) {
            byte[] buf = new byte[7];
            buf[0] = (byte)'$';
            buf[1] = (byte)'R';
            buf[2] = (byte)'R';
            buf[3] = reg;
            buf[4] = (byte)'@';
            //buf[5] = ;
            buf[6] = (byte)'#';

            byte cs = 0;
            for (int i = 1; i <= 4; i++)
                cs = (byte)((int)cs ^ buf[i]);
            buf[5] = cs;
            // откладываем запуск до момента готовности принять ответ
            //com.Write(buf, 0, buf.Length);

            bool complete = false;
            byte res = 0;
            ReadRegResultCommand.RecievedReadRegResult +=
                delegate (ReadRegResultCommand rrrc, byte result) {
                    res = result;
                    complete = true;
                };
            // отложеный запуск
            com.Write(buf, 0, buf.Length);

            while (!complete)
                Thread.Sleep(5);
            return res;
        }

        public void WriteReg(byte reg, byte val) {
            byte[] buf = new byte[8];
            buf[0] = (byte)'$';
            buf[1] = (byte)'R';
            buf[2] = (byte)'W';
            buf[3] = reg;
            buf[4] = val;
            buf[5] = (byte)'@';
            //buf[6] = ;
            buf[7] = (byte)'#';

            byte cs = 0;
            for (int i = 1; i <= 5; i++)
                cs = (byte)((int)cs ^ buf[i]);
            buf[6] = cs;
            com.Write(buf, 0, buf.Length);
        }

        public void WriteReg3(byte reg1, byte reg2, byte reg3, uint val) {
            WriteReg(reg1, (byte)((val >> 16) & 0xFF));
            WriteReg(reg2, (byte)((val >>  8) & 0xFF));
            WriteReg(reg3, (byte)( val        & 0xFF));
        }

        public uint SPICommonCommand(CommondCommandType cmd_type, byte cmd_code, byte num_reads, byte num_writes, uint write_value) {
            num_reads &= 3;
            num_writes &= 3;
            write_value &= 0x00FFFFFF;
            byte reg_value = (byte)(((int)cmd_type << 5) |
                                    (num_writes << 3) |
                                    (num_reads << 1));

            WriteReg(0x60, reg_value);
            WriteReg(0x61, cmd_code);
            switch (num_writes) {
                case 3:
                    //WriteReg(0x64, (byte)((write_value >> 16) & 0xFF));
                    //WriteReg(0x65, (byte)((write_value >> 8) & 0xFF));
                    //WriteReg(0x66, (byte)(write_value & 0xFF));
                    WriteReg3(0x64, 0x65, 0x66, write_value);
                    break;
                case 2:
                    WriteReg(0x64, (byte)((write_value >> 8) & 0xFF));
                    WriteReg(0x65, (byte)(write_value & 0xFF));
                    break;
                case 1:
                    WriteReg(0x64, (byte)(write_value & 0xFF));
                    break;
            }
            WriteReg(0x60, (byte)(reg_value | 1)); // Execute the command

            byte b;
            do {
                b = ReadReg(0x60);
            } while ((b & 1) != 0);  // TODO: add timeout and reset the controller
            switch (num_reads) {
                case 0:
                    return 0;
                case 1:
                    return ReadReg(0x67);
                case 2:
                    return (uint)(ReadReg(0x67) << 8) | ReadReg(0x68);
                case 3:
                    return (uint)(ReadReg(0x67) << 16) | (uint)(ReadReg(0x68) << 8) | ReadReg(0x69);
            }
            return 0;
        }

        bool ReadBytesFromAddr(byte reg, byte[] dst, int pos, byte len) {
            byte[] buf = new byte[8];
            buf[0] = (byte)'$';
            buf[1] = (byte)'B';
            buf[2] = (byte)'R';
            buf[3] = reg;
            buf[4] = len;
            buf[5] = (byte)'@';
            //buf[6] = ;
            buf[7] = (byte)'#';

            byte cs = 0;
            for (int i = 1; i <= 5; i++)
                cs = (byte)((int)cs ^ buf[i]);
            buf[6] = cs;
            // откладываем запуск до момента готовности приенять ответ
            //com.Write(buf, 0, buf.Length);


            bool complete = false;
            byte[] res = null;
            int len2 = 0;
            ReadBytesResultCommand.RecievedReadBytesResult +=
                delegate (ReadBytesResultCommand rrrc, byte[] result, int length) {
                    res = result;
                    len2 = length;
                    complete = true;
                };
            // отложеная отправка
            com.Write(buf, 0, buf.Length);



            while (!complete)
                Thread.Sleep(5);
            Array.Copy(res, 0, dst, pos, len2);
            return len2 == len;
        }

        void SPIRead(uint address, byte[] data, int len) {
            WriteReg(0x60, 0x46);
            WriteReg(0x61, 0x3);
            //WriteReg(0x64, (byte)((address >> 16) & 0xFF));
            //WriteReg(0x65, (byte)((address >> 8) & 0xFF));
            //WriteReg(0x66, (byte)(address & 0xFF));
            WriteReg3(0x64, 0x65, 0x66, address);
            WriteReg(0x60, 0x47); // Execute the command
            byte b;
            do {
                b = ReadReg(0x60);
            } while ((b & 1) != 0);  // TODO: add timeout and reset the controller
            int pos = 0;
            while (len > 0) {
                int read_len = len;
                if (read_len > 64)
                    read_len = 64;
                ReadBytesFromAddr(0x70, data, pos, (byte)read_len);
                pos += read_len;
                len -= read_len;
            }
        }

        bool WriteBytesToAddr(byte reg, byte[] values, int from, byte len) {
            byte[] buf = new byte[8+len];
            buf[0] = (byte)'$';
            buf[1] = (byte)'B';
            buf[2] = (byte)'W';
            buf[3] = reg;
            buf[4] = len;
            for (int i = 0; i < len; i++) {
                buf[5 + i] = values[from+i];
            }
            buf[5+len] = (byte)'@';
            //buf[6+len] = ;
            buf[7+len] = (byte)'#';

            byte cs = 0;
            for (int i = 1; i <= 4+len; i++)
                cs = (byte)((int)cs ^ buf[i]);
            buf[6+len] = cs;
            com.Write(buf, 0, buf.Length);
            return true;
        }
    }
}
