namespace TerminalStress
{
    using System;
    using System.Text;
	using Microsoft.Extensions.Logging;

    class Program
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        static void Main(string[] args)
        {
		_logger.LogDebug(">>> Entering Main(args={Args})", args);

            Random r = new Random();
		_logger.LogDebug("<<< Exiting Main(args={Args})", args);
			_logger.LogDebug("Main(args={Args}): r → {R}", args, r);

#pragma warning disable SYSLIB0001
            Console.OutputEncoding = args.Length > 0 ? Encoding.UTF7 : Encoding.UTF8;
			_logger.LogDebug("Main(args={Args}): Console.OutputEncoding → {Arg1}", args, Console.OutputEncoding);
            string s = string.Empty;
			_logger.LogDebug("Main(args={Args}): s → {S}", args, s);

            var colors = new ConsoleColor[]
            {
                ConsoleColor.Black,
                ConsoleColor.Red,
                ConsoleColor.Green,
                ConsoleColor.Blue,
                ConsoleColor.Cyan,
                ConsoleColor.Magenta,
                ConsoleColor.Yellow,
                ConsoleColor.White,
                ConsoleColor.DarkYellow,
                ConsoleColor.DarkCyan,
                ConsoleColor.DarkBlue,
                ConsoleColor.DarkGray,
                ConsoleColor.DarkGreen,
                ConsoleColor.DarkMagenta,
                ConsoleColor.DarkRed
            };

            while (true)
            {
                try
                {
                    Console.SetCursorPosition(r.Next(Console.WindowWidth), r.Next(Console.WindowHeight));
                }
                catch
                {
                    Console.Write("☠️");
                }

                try
                {
                    var color = colors[r.Next(colors.Length)];
					_logger.LogDebug("Main(args={Args}): color → {Color}", args, color);
					_logger.LogDebug("Main(args={Args}): Console.ForegroundColor → {Arg1}", args, Console.ForegroundColor);
                    Console.ForegroundColor = color;

                    char c = (char)r.Next(0, 0xFFFF);
					_logger.LogDebug("Main(args={Args}): c → {C}", args, c);
                    Console.Write(c);
                    s += c;

                    if (r.Next(1_000) == 1)
                    {
                        Console.Clear();
                        Console.WriteLine(s);
                    }

                    if (s.Length > 1_000)
                    {
                        s = string.Empty;
                    }

                    if (r.Next(1_000_000) < 100)
                    {
                        for (int i = 0; i < 100; i++)
                        {
                            Console.Write("☠️");
                        }
                    }
                }
                catch
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.Write("👀🤬💀👀👀💀");
                }
            }
        }
    }
}
