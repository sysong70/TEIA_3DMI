using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.Window;

namespace RichText
{
	public class ColorKeyword
	{
		public Color Color { get; set; } = Color.White;
		public List<string> Keywords { get; set; } = new List<string>();

		public ColorKeyword(string value) {
			Set(value);
		}

		public ColorKeyword(Color color, string keywords) {
			Color = color;
			SetKeywords(keywords);
		}

		public ColorKeyword(Color color, string[] keywords) {
			Color = color;
			Keywords.AddRange(new List<string>(keywords));
		}

		public string Get() {
			return $"{GetColor()}#{GetKeywords()}";
		}

		public string GetColor() {
			return $"{Color.R.ToString()},{Color.G.ToString()},{Color.B.ToString()}";
		}

		public string GetKeywords() {
			return string.Join(" ", Keywords);
		}

		public void Set(string value) {
			string[] colorAndKeywords = value.Split('#');
			SetColor(colorAndKeywords[0]);
			SetKeywords(colorAndKeywords[1]);
		}

		public void SetColor(string color) {
			string[] rgb = color.Split(',');
			Color = Color.FromArgb(int.Parse(rgb[0]), int.Parse(rgb[1]), int.Parse(rgb[2]));
		}

		public void SetKeywords(string keywords) {
			char[] empty = { ' ' };
			Keywords = keywords.Split(empty, StringSplitOptions.RemoveEmptyEntries).ToList<string>().Distinct().ToList();
		}

		public void AddKeyword(string keyword) {
			if (Keywords.Find(x => x == keyword) != null) {
				Keywords.Add(keyword);
			}
		}
	}



	public class Handler
	{
		// Not character space, pixel size
		public int TabSpace { get; set; } = 28;
		public List<ColorKeyword> ColorKeywords = new List<ColorKeyword>();
		public RichTextBox Target = null;

		public Handler(RichTextBox target) {
			Target = target;

			List<int> tabs = new List<int>();
			for (int tab = TabSpace; tab < 32; tab += TabSpace) {
				tabs.Add(tab);
			}
			Target.SelectionTabs = tabs.ToArray();
			Target.ForeColor = Color.WhiteSmoke;
		}

		public string GetColorKeywords() {
			string buffer = string.Empty;
			return string.Join(";", ColorKeywords);
		}

		public void SetColorKeywords(string value) {
			string[] items = value.Split(';');
			foreach (var item in items) {
				ColorKeywords.Add(new ColorKeyword(item));
			}
		}

		public void AddText(string stream) {
			Regex r = new Regex("([ \\t\\n{}():;])");
			string[] tokens = r.Split(stream.Replace("\r", ""));

			foreach (string token in tokens) {
				// Set the tokens default color and font.  
				Target.SelectionColor = Color.White;
				// Check whether the token is a keyword.   
				foreach (var value in ColorKeywords) {
					foreach (var keyword in value.Keywords) {
						if (keyword == token) {
							Target.SelectionColor = value.Color;
						}
					}
				}
				Target.SelectedText = token;
			}
		}

		public void Refresh() {
			string stream = Target.Text;
			Target.Clear();

			AddText(stream);
		}
	}
}
