using System;
using System.Drawing;
using System.Linq;
using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace Tracer
{
	public partial class ServerForm : Form {
		public Color Foreground = Color.FromArgb(216, 222, 233);
		public Color Background = Color.FromArgb(30, 30, 30);
		public Configure Option = new Configure();
		public RichText.Handler Handler = null;

		private delegate void ReceiveDelegate(byte[] buffer, int read);
		private ReceiveDelegate ReceiveData;

		public ServerForm() {
			InitializeComponent();

			ReceivedTextBox.ForeColor = Foreground;
			ReceivedTextBox.BackColor = Background;

			Load += (sender, e) => {
				Option.Load();

				string[] args = Environment.GetCommandLineArgs();
				if (args.Length > 1) {
					Option.AddressAndPort = args[1];
				}

				string addressAndPort = Option.AddressAndPort;
				if (string.IsNullOrEmpty(addressAndPort) == false) {
					AddressTextBox.Text = addressAndPort;
				}

				FilterTextBox.Text = string.Join(" ", Option.Filters);
				MessageHandler.ReceivedTextBox = ReceivedTextBox;
				MessageHandler.Filters = Option.Filters;
				ReceiveData = MessageHandler.SaveOnTextBox;
			};

			FormClosed += (sender, e) => {
				Option.Save();
			};

			ListenButton.Click += (sender, e) => {
				AddressTextBox.Enabled = false;

				Option.Address = AddressTextBox.Text.Split(':')[0].Trim();
				Option.Port = int.Parse(AddressTextBox.Text.Split(':')[1].Trim());
				Task.Factory.StartNew(StartListening);
			};

			FilterTextBox.Leave += (sender, e) => {
				MessageHandler.Filters = Option.Filters = FilterTextBox.Text.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
			};

			FileModeCheckBox.CheckStateChanged += (sender, e) => {
				CheckBox fileMode = sender as CheckBox;
				if (fileMode.Checked) {
					ReceiveData = MessageHandler.SaveOnStringBuilder;
				}
				else {
					ReceiveData = MessageHandler.SaveOnTextBox;
				}
			};

			SaveButton.Click += (sender, e) => {
				SaveFileDialog dialog = new SaveFileDialog();
				if (dialog.ShowDialog() == DialogResult.OK) {
					File.WriteAllText(dialog.FileName, MessageHandler.Builder.ToString());
					MessageHandler.Builder.Clear();
				}
			};
		}

		void StartListening() {
			byte[] buffer = new byte[4096];
			TcpListener listener = new TcpListener(IPAddress.Parse(Option.Address), Option.Port);
			listener.Start();

			while (true) {
				TcpClient client = listener.AcceptTcpClient();
				NetworkStream stream = client.GetStream();

				int read;
				while ((read = stream.Read(buffer, 0, buffer.Length)) > 0) {
					ReceiveData(buffer, read);
				}

				stream.Close();
				client.Close();
			}
		}
	}



	public class MessageHandler {
		public static TextBox ReceivedTextBox;
		public static StringBuilder Builder = new StringBuilder();
		public static string[] Filters;
		public static int TickCount = 0;

		public static void SaveOnTextBox(byte[] buffer, int read) {
			ReceivedTextBox.Invoke((MethodInvoker)delegate {
				string encode = Encoding.UTF8.GetString(buffer, 0, read);
				string[] lines = encode.Split('\n');
				foreach (string line in lines) {
					if (Filters.Any(x => line.Contains(x)) == false) {
						ReceivedTextBox.AppendText(line);
						ReceivedTextBox.AppendText(Environment.NewLine);
					}
				}
			});
		}

		public static void SaveOnStringBuilder(byte[] buffer, int read) {
			string encode = Encoding.UTF8.GetString(buffer, 0, read);
			string[] lines = encode.Split('\n');
			foreach (string line in lines) {
				if (Filters.Any(x => line.Contains(x)) == false) {
					Builder.Append(line);
					Builder.Append(Environment.NewLine);
				}
			}

			ReceivedTextBox.Invoke((MethodInvoker)delegate {
				ReceivedTextBox.Text = $"Received {++TickCount} - {encode}";
			});
		}
	}



	public class Configure
	{
		private readonly string FileName = "tracer.ini";
		// Not domain, IP address
		public string AddressAndPort {
			get {
				return string.IsNullOrEmpty(Address) ? "" : $"{Address}:{Port}";
			}
			set {
				Address = value.Split(':')[0];
				Port = int.Parse(value.Split(':')[1]);
			}
		}
		public string Address { get; set; } = "127.0.0.1";
		public int Port { get; set; } = 8080;
		public string[] Filters = null;

		public bool Load() {
			string[] stream = File.ReadAllText(FileName).Split('\n');
			if (stream != null && stream.Length == 2) {
				AddressAndPort = stream[0];
				Filters = stream[1].Split(new char[]{ ' ' }, StringSplitOptions.RemoveEmptyEntries);
				return true;
			}
			else {
				return false;
			}
		}

		public void Save() {
			File.WriteAllText(FileName, $"{AddressAndPort}\n{string.Join(" ", Filters)}");		}
	}
}
