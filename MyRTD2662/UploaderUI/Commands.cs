using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UploaderUI {
    public abstract class Command {
        protected Communication communication;

        public Command (Communication communication) {
            this.communication = communication;
        }

        public abstract bool Read();

        public abstract void Run();
    }

    public class VersionCommand : Command {
        public string V1 { get; protected set; }
        public string V2 { get; protected set; }
        public string V3 { get; protected set; }
        public string V4 { get; protected set; }

        public VersionCommand(Communication communication) : base(communication) {
        }

        public override bool Read() {
            V1 = communication.ReadToDelemiter('.', 3);
            V2 = communication.ReadToDelemiter('.', 3);
            V3 = communication.ReadToDelemiter('.', 4);
            V4 = communication.ReadToDelemiter('.', 5);
            return true;
        }

        public override void Run() {
            communication.Log("Версия прошивальщика: {0}.{1}.{2}.{3}.", Level.Info, V1, V2, V3, V4);
        }
    }

    public class MessageCommand : Command {
        string msg = "";
        public MessageCommand(Communication communication) : base(communication) {
        }

        public override bool Read() {
            int len = communication.COMReadUShort();
            for (int i = 0; i < len; i++) {
                msg += communication.COMReadChar();
            }
            return true;
        }

        public override void Run() {
            communication.Log("Message: {0}", Level.Info, msg);
        }
    }

    public class ReadRegResultCommand : Command {
        public byte ReadRegResult { get; protected set; }

        public delegate void RecievedReadRegResultDelegate(ReadRegResultCommand rrrc, byte result);

        public static event RecievedReadRegResultDelegate RecievedReadRegResult;

        public ReadRegResultCommand(Communication communication) : base(communication) {
        }

        public override bool Read() {
            ReadRegResult = communication.COMReadByte();
            return true;
        }

        public override void Run() {
            if (RecievedReadRegResult != null)
                RecievedReadRegResult.Invoke(this, ReadRegResult);
            RecievedReadRegResult = null;
        }
    }

    public class ReadBytesResultCommand : Command {
        public byte Length { get; protected set; }
        public byte[] ReadBytesResult { get; protected set; }

        public delegate void RecievedReadBytesResultDelegate(ReadBytesResultCommand rrrc, byte[] result, int length);

        public static event RecievedReadBytesResultDelegate RecievedReadBytesResult;

        public ReadBytesResultCommand(Communication communication) : base(communication) {
        }

        public override bool Read() {
            Length = communication.COMReadByte();
            ReadBytesResult = new byte[Length];
            for (int i = 0; i < Length; i++)
                ReadBytesResult[i] = communication.COMReadByte();
            return true;
        }

        public override void Run() {
            if (RecievedReadBytesResult != null)
                RecievedReadBytesResult.Invoke(this, ReadBytesResult, Length);
            RecievedReadBytesResult = null;
        }
    }
}
