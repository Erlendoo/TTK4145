using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace Client.Simulator
{

    public enum MotorDirection
    {
        Down = -1,
        Stop = 0,
        Up = 1
    }

    public enum ButtonType
    {
        Up = 0,
        Down = 1,
        Command = 2
    }

    internal class StateMachine
    {
        readonly NetworkStream _stream;
        readonly BinaryWriter _binaryWriter;
        readonly BinaryReader _binaryReader;

        public List<ButtonType> Buttons = Enum.GetValues(typeof(ButtonType)).Cast<ButtonType>().ToList();

        public short Floors { get; }
        public short FloorStart = 0;
        public short FloorEnd => (short) (Floors - 1);

        public StateMachine(NetworkStream stream, short floors)
        {
            if (stream == null) throw new ArgumentNullException(nameof(stream));
            if (floors <= 0) throw new ArgumentOutOfRangeException(nameof(floors));

            _stream = stream;
            _binaryWriter = new BinaryWriter(_stream);
            _binaryReader = new BinaryReader(_stream, Encoding.ASCII, true);

            Floors = floors;
        }

        public void Init()
        {
            for (var i = 0; i < Floors; i++)
            {
                foreach (var buttonType in Enum.GetValues(typeof(ButtonType)).Cast<ButtonType>())
                {
                    SetButtonLamp(buttonType, i, 0);
                }
            }

            SetStopLamp(0);
            SetDoorOpenLamp(0);
            SetFloorIndicator(0);
        }

        public void Reload()
        {
            Write(0);
        }

        public void SetButtonLamp(ButtonType buttonType, int floor, int value)
        {
            _stream.Write(new byte[] { 2, (byte)buttonType, (byte)floor, (byte)value }, 0, 4);
        }

        public void SetStopLamp(int value)
        {
            Write(5, value);
        }

        public void SetDoorOpenLamp(int value)
        {
            Write(5, value);
        }

        public void SetFloorIndicator(int floor)
        {
            Write(3, floor);
        }

        public void SetMotorDirection(MotorDirection direction)
        {
            Write(1, (int)direction);
        }

        public void Order(ButtonType buttonType, int floor)
        {
            Write(2, (int)buttonType, floor);
        }

        public short GetCurrentFloor()
        {
            Write(7, 4);

            var values = _binaryReader.ReadBytes(4);

            if (values[1] > 0)
            {
                return Convert.ToInt16(values[2]);
            }

            return -1;
        }

        void Write(params int[] values)
        {
            if (values.Length > 3)
            {
                throw new ArgumentOutOfRangeException();
            }

            var command = new byte[]
            {
                (byte) (values.Length > 0 ? values[0] : 0),
                (byte) (values.Length > 1 ? values[1] : 0),
                (byte) (values.Length > 2 ? values[2] : 0),
                0
            };

            _binaryWriter.Write(command, 0, 4);

        }

    }

    class Program
    {
        static void Main()
        {
            var client = new TcpClient();
            client.Connect(new IPEndPoint(IPAddress.Loopback, 15657));

            var stateMachine = new StateMachine(client.GetStream(), 4);
            stateMachine.Init();

            var initialized = false;
            var previousFloor = -1;

            while (true)
            {
                var floor = stateMachine.GetCurrentFloor();

                if (initialized)
                {
                    if (floor == -1 || 
                        floor == previousFloor ||
                        floor > stateMachine.FloorStart &&
                        floor < stateMachine.FloorEnd)
                    {
                        if (floor != -1 && floor != previousFloor)
                        {
                            stateMachine.SetFloorIndicator(floor);
                        }
                        previousFloor = floor;
                        continue;
                    }
                }
                else
                {
                    floor = floor == -1 ? stateMachine.FloorStart : stateMachine.FloorEnd;
                }

                initialized = true;

                stateMachine.SetMotorDirection(MotorDirection.Stop);

                if (floor == stateMachine.FloorStart)
                {
                    stateMachine.SetFloorIndicator(stateMachine.FloorStart);
                    stateMachine.Order(ButtonType.Up, stateMachine.FloorStart);
                    stateMachine.SetMotorDirection(MotorDirection.Up);
                    previousFloor = stateMachine.FloorStart;
                }
                else if (floor == stateMachine.FloorEnd)
                {
                    stateMachine.SetFloorIndicator(stateMachine.FloorEnd);
                    stateMachine.Order(ButtonType.Down, stateMachine.FloorEnd);
                    stateMachine.SetMotorDirection(MotorDirection.Down);
                    previousFloor = stateMachine.FloorEnd;
                }
                else
                {
                    throw new Exception("Bad state");
                }
            }

        }
    }
}
