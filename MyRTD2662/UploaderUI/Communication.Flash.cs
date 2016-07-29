using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UploaderUI {
    public partial class Communication {
        public struct FlashDesc {
            public FlashDesc(string device_name, uint jedec_id, uint size_kb, uint page_size, uint block_size_kb) {
                this.device_name = device_name;
                this.jedec_id = jedec_id;
                this.size_kb = size_kb;
                this.page_size = page_size;
                this.block_size_kb = block_size_kb;
            }

            public string device_name;
            public uint jedec_id;
            public uint size_kb;
            public uint page_size;
            public uint block_size_kb;
        };

        static List<FlashDesc> FlashDevices = new List<FlashDesc>() {
            //              name,        Jedec ID,    sizeK, page size, block sizeK
            new FlashDesc("AT25DF041A" , 0x1F4401,      512,       256, 64),
            new FlashDesc("AT25DF161"  , 0x1F4602, 2 * 1024,       256, 64),
            new FlashDesc("AT26DF081A" , 0x1F4501, 1 * 1024,       256, 64),
            new FlashDesc("AT26DF0161" , 0x1F4600, 2 * 1024,       256, 64),
            new FlashDesc("AT26DF161A" , 0x1F4601, 2 * 1024,       256, 64),
            new FlashDesc("AT25DF321" ,  0x1F4701, 4 * 1024,       256, 64),
            new FlashDesc("AT25DF512B" , 0x1F6501,       64,       256, 32),
            new FlashDesc("AT25DF512B" , 0x1F6500,       64,       256, 32),
            new FlashDesc("AT25DF021"  , 0x1F3200,      256,       256, 64),
            new FlashDesc("AT26DF641" ,  0x1F4800, 8 * 1024,       256, 64),
            // Manufacturer: ST 
            new FlashDesc("M25P05"     , 0x202010,       64,       256, 32),
            new FlashDesc("M25P10"     , 0x202011,      128,       256, 32),
            new FlashDesc("M25P20"     , 0x202012,      256,       256, 64),
            new FlashDesc("M25P40"     , 0x202013,      512,       256, 64),
            new FlashDesc("M25P80"     , 0x202014, 1 * 1024,       256, 64),
            new FlashDesc("M25P16"     , 0x202015, 2 * 1024,       256, 64),
            new FlashDesc("M25P32"     , 0x202016, 4 * 1024,       256, 64),
            new FlashDesc("M25P64"     , 0x202017, 8 * 1024,       256, 64),
            // Manufacturer: Windbond 
            new FlashDesc("W25X10"     , 0xEF3011,      128,       256, 64),
            new FlashDesc("W25X20"     , 0xEF3012,      256,       256, 64),
            new FlashDesc("W25X40"     , 0xEF3013,      512,       256, 64),
            new FlashDesc("W25X80"     , 0xEF3014, 1 * 1024,       256, 64),
            // Manufacturer: Macronix 
            new FlashDesc("MX25L512"   , 0xC22010,       64,       256, 64),
            new FlashDesc("MX25L3205"  , 0xC22016, 4 * 1024,       256, 64),
            new FlashDesc("MX25L6405"  , 0xC22017, 8 * 1024,       256, 64),
            new FlashDesc("MX25L8005"  , 0xC22014,     1024,       256, 64),
            // Microchip
            new FlashDesc("SST25VF512" , 0xBF4800,       64,       256, 32),
            new FlashDesc("SST25VF032" , 0xBF4A00, 4 * 1024,       256, 32),
            new FlashDesc(null , 0, 0, 0, 0)
        };

        public static FlashDesc FindFlashDesc(uint jedec_id) {
            return FlashDevices.FirstOrDefault(fd => fd.jedec_id == jedec_id);
        }

        public static string JEDECManufacturer(uint jedec_id) {
            switch (jedec_id >> 16) {
                case 0x20:
                    return "ST";
                case 0xef:
                    return "Winbond";
                case 0x1f:
                    return "Atmel";
                case 0xc2:
                    return "Macronix";
                case 0xbf:
                    return "Microchip";
                default:
                    return "Unknown";
            }
        }

        public void SetupChipCommands(uint jedec_id) {
            byte manufacturer_id = (byte)(jedec_id >> 16);
            switch (manufacturer_id) {
                case 0xEF: // These are the codes for Winbond
                    WriteReg(0x62, 0x06);  // Flash Write enable op code
                    WriteReg(0x63, 0x50); // Flash Write register op code
                    WriteReg(0x6a, 0x03);  // Flash Read op code.
                    WriteReg(0x6b, 0x0b);  // Flash Fast read op code.
                    WriteReg(0x6d, 0x02);  // Flash program op code.
                    WriteReg(0x6e, 0x05);  // Flash read status op code.
                    break;
                default:
                    Log("Can not handle manufacturer code {0}", Level.Error, manufacturer_id);
                    break;
            }
        }

        UInt16 crc = 0;
        void ProcessCRC(byte[] buf, int len) {
            int i, j;
            for (j = 0; j < len; j++) {
                byte data = buf[j];
                crc ^= (ushort)(data << 8);
                for (i = 0; i < 8; i++) {
                    if ((crc & 0x8000) != 0)
                        crc ^= (0x1070 << 3);
                    crc <<= 1;
                }
            }
        }

        byte SPIComputeCRC(uint start, uint end) {
            //WriteReg(0x64, (byte)((start >> 16) & 0xFF));
            //WriteReg(0x65, (byte)((start >> 8) & 0xFF));
            //WriteReg(0x66, (byte)(start & 0xFF));
            WriteReg3(0x64, 0x65, 0x66, start);

            //WriteReg(0x72, (byte)((end >> 16) & 0xFF));
            //WriteReg(0x73, (byte)((end >> 8) & 0xFF));
            //WriteReg(0x74, (byte)(end & 0xFF));
            WriteReg3(0x72, 0x73, 0x74, end);

            WriteReg(0x6f, 0x84);
            byte b;
            do {
                b = ReadReg(0x6f);
            } while ((b & 0x2) == 0);  // TODO: add timeout and reset the controller
            return ReadReg(0x75);
        }


        public bool SaveFlash(string output_file_name, uint chip_size) {
            FileStream fs = new FileStream(output_file_name, FileMode.Create);

            uint addr = 0;
            crc = 0; // InitCRC();
            do {
                byte[] buffer = new byte[64];
                Console.WriteLine("Writing addr {0:X}", addr);
                SPIRead(addr, buffer, buffer.Length);
                fs.Write(buffer, 0, buffer.Length);
                ProcessCRC(buffer, buffer.Length);
                addr += (uint)buffer.Length;
                fs.Flush();
            } while (addr < chip_size);
            Log("done", Level.Info);

            fs.Flush();
            fs.Close();

            byte data_crc = (byte)(crc >> 8);//GetCRC();
            byte chip_crc = SPIComputeCRC(0, chip_size - 1);
            Log("Received data CRC {0}", Level.Info, data_crc);
            Log("Chip CRC {0}", Level.Info, chip_crc);
            return data_crc == chip_crc;
        }

        bool ShouldProgramPage(byte[] buffer, uint size) {
            for (uint idx = 0; idx < size; ++idx) {
                if (buffer[idx] != 0xff)
                    return true;
            }
            return false;
        }

        public bool ProgramFlash(string input_file_name, uint chip_size) {
            byte[] prog = File.ReadAllBytes(input_file_name);
            uint prog_size = (uint)prog.Length; // (uint)new FileInfo(input_file_name).Length;

            Log("Erasing...", Level.Info);
            SPICommonCommand(CommondCommandType.E_CC_WRITE_AFTER_EWSR, 1, 0, 1, 0); // Unprotect the Status Register
            SPICommonCommand(CommondCommandType.E_CC_WRITE_AFTER_WREN, 1, 0, 1, 0); // Unprotect the flash
            SPICommonCommand(CommondCommandType.E_CC_ERASE, 0xc7, 0, 0, 0);         // Chip Erase
            Log("done\n", Level.Info);

            byte[] buffer = new byte[256];
            uint addr = 0;
            uint data_pos = 0;
            uint data_len = prog_size;
            crc = 0;
            byte b = 0xFF;
            do {
                // Wait for programming cycle to finish
                do {
                    b = ReadReg(0x6f);
                } while ((b & 0x40) != 0);

                Console.WriteLine("Writing addr {0:X}", addr);
                // Fill with 0xff in case we read a partial buffer.
                for (int i = 0; i < buffer.Length; i++)
                    buffer[i] = 0xFF;
                uint len = (uint)buffer.Length;
                if (len > data_len) {
                    len = data_len;
                }
                Array.Copy(prog, data_pos, buffer, 0, len);
                data_pos += len;
                data_len -= len;

                if (ShouldProgramPage(buffer, (uint)buffer.Length)) {
                    // Set program size-1
                    WriteReg(0x71, 255);

                    // Set the programming address
                    //WriteReg(0x64, addr >> 16);
                    //WriteReg(0x65, addr >> 8);
                    //WriteReg(0x66, addr);
                    WriteReg3(0x64, 0x65, 0x66, addr);

                    // Write the content to register 0x70
                    // Out USB gizmo supports max 63 bytes at a time.
                    WriteBytesToAddr(0x70, buffer, 64*0, 64);
                    WriteBytesToAddr(0x70, buffer, 64*1, 64);
                    WriteBytesToAddr(0x70, buffer, 64*2, 64);
                    WriteBytesToAddr(0x70, buffer, 64*3, 64);
                    //WriteBytesToAddr(0x70, buffer + 252, 4);

                    WriteReg(0x6f, 0xa0); // Start Programing
                }
                ProcessCRC(buffer, buffer.Length);
                addr += (uint)buffer.Length;
            } while (addr < chip_size && data_len != 0);

            // Wait for programming cycle to finish
            do {
                b = ReadReg(0x6f);
            } while ((b & 0x40) != 0);

            SPICommonCommand(CommondCommandType.E_CC_WRITE_AFTER_EWSR, 1, 0, 1, 0x1c); // Unprotect the Status Register
            SPICommonCommand(CommondCommandType.E_CC_WRITE_AFTER_WREN, 1, 0, 1, 0x1c); // Protect the flash

            byte data_crc = (byte)(crc >> 8);//GetCRC();
            byte chip_crc = SPIComputeCRC(0, addr - 1);
            Log("Received data CRC {0:X}", Level.Info, data_crc);
            Log("Chip CRC {0:X}", Level.Info, chip_crc);
            return data_crc == chip_crc;
        }
    }
}
