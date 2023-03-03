namespace Tracer
{
	partial class ServerForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing) {
			if (disposing && (components != null)) {
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
			this.ReceivedTextBox = new System.Windows.Forms.TextBox();
			this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
			this.Label1 = new System.Windows.Forms.Label();
			this.AddressTextBox = new System.Windows.Forms.TextBox();
			this.FileModeCheckBox = new System.Windows.Forms.CheckBox();
			this.ListenButton = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.FilterTextBox = new System.Windows.Forms.TextBox();
			this.SaveButton = new System.Windows.Forms.Button();
			this.tableLayoutPanel1.SuspendLayout();
			this.tableLayoutPanel2.SuspendLayout();
			this.SuspendLayout();
			// 
			// tableLayoutPanel1
			// 
			this.tableLayoutPanel1.ColumnCount = 1;
			this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.Controls.Add(this.ReceivedTextBox, 0, 1);
			this.tableLayoutPanel1.Controls.Add(this.tableLayoutPanel2, 0, 0);
			this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
			this.tableLayoutPanel1.Name = "tableLayoutPanel1";
			this.tableLayoutPanel1.Padding = new System.Windows.Forms.Padding(6);
			this.tableLayoutPanel1.RowCount = 2;
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 60F));
			this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
			this.tableLayoutPanel1.Size = new System.Drawing.Size(1350, 1144);
			this.tableLayoutPanel1.TabIndex = 1;
			// 
			// ReceivedTextBox
			// 
			this.ReceivedTextBox.AcceptsReturn = true;
			this.ReceivedTextBox.AcceptsTab = true;
			this.ReceivedTextBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.ReceivedTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
			this.ReceivedTextBox.Location = new System.Drawing.Point(12, 72);
			this.ReceivedTextBox.Margin = new System.Windows.Forms.Padding(6);
			this.ReceivedTextBox.Multiline = true;
			this.ReceivedTextBox.Name = "ReceivedTextBox";
			this.ReceivedTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
			this.ReceivedTextBox.Size = new System.Drawing.Size(1326, 1060);
			this.ReceivedTextBox.TabIndex = 2;
			this.ReceivedTextBox.WordWrap = false;
			// 
			// tableLayoutPanel2
			// 
			this.tableLayoutPanel2.BackColor = System.Drawing.SystemColors.Control;
			this.tableLayoutPanel2.ColumnCount = 8;
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
			this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 101F));
			this.tableLayoutPanel2.Controls.Add(this.Label1, 0, 0);
			this.tableLayoutPanel2.Controls.Add(this.AddressTextBox, 1, 0);
			this.tableLayoutPanel2.Controls.Add(this.FileModeCheckBox, 5, 0);
			this.tableLayoutPanel2.Controls.Add(this.ListenButton, 2, 0);
			this.tableLayoutPanel2.Controls.Add(this.label2, 3, 0);
			this.tableLayoutPanel2.Controls.Add(this.FilterTextBox, 4, 0);
			this.tableLayoutPanel2.Controls.Add(this.SaveButton, 6, 0);
			this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tableLayoutPanel2.Location = new System.Drawing.Point(9, 9);
			this.tableLayoutPanel2.Name = "tableLayoutPanel2";
			this.tableLayoutPanel2.RowCount = 1;
			this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle());
			this.tableLayoutPanel2.Size = new System.Drawing.Size(1332, 54);
			this.tableLayoutPanel2.TabIndex = 3;
			// 
			// Label1
			// 
			this.Label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
			this.Label1.AutoSize = true;
			this.Label1.ForeColor = System.Drawing.SystemColors.HotTrack;
			this.Label1.Location = new System.Drawing.Point(0, 14);
			this.Label1.Margin = new System.Windows.Forms.Padding(0);
			this.Label1.Name = "Label1";
			this.Label1.Size = new System.Drawing.Size(54, 25);
			this.Label1.TabIndex = 1;
			this.Label1.Text = "From";
			// 
			// AddressTextBox
			// 
			this.AddressTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
			this.AddressTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.AddressTextBox.Location = new System.Drawing.Point(64, 15);
			this.AddressTextBox.Margin = new System.Windows.Forms.Padding(10, 0, 0, 0);
			this.AddressTextBox.Name = "AddressTextBox";
			this.AddressTextBox.Size = new System.Drawing.Size(200, 24);
			this.AddressTextBox.TabIndex = 2;
			this.AddressTextBox.Text = "127.0.0.1:8080";
			// 
			// FileModeCheckBox
			// 
			this.FileModeCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
			this.FileModeCheckBox.AutoSize = true;
			this.FileModeCheckBox.Location = new System.Drawing.Point(943, 12);
			this.FileModeCheckBox.Margin = new System.Windows.Forms.Padding(30, 0, 0, 0);
			this.FileModeCheckBox.Name = "FileModeCheckBox";
			this.FileModeCheckBox.Size = new System.Drawing.Size(120, 29);
			this.FileModeCheckBox.TabIndex = 5;
			this.FileModeCheckBox.Text = "File Mode";
			this.FileModeCheckBox.UseVisualStyleBackColor = true;
			// 
			// ListenButton
			// 
			this.ListenButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
			this.ListenButton.AutoSize = true;
			this.ListenButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.ListenButton.Location = new System.Drawing.Point(274, 8);
			this.ListenButton.Margin = new System.Windows.Forms.Padding(10, 0, 0, 0);
			this.ListenButton.Name = "ListenButton";
			this.ListenButton.Size = new System.Drawing.Size(75, 37);
			this.ListenButton.TabIndex = 11;
			this.ListenButton.Text = "Listen";
			this.ListenButton.UseVisualStyleBackColor = true;
			// 
			// label2
			// 
			this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
			this.label2.AutoSize = true;
			this.label2.ForeColor = System.Drawing.SystemColors.HotTrack;
			this.label2.Location = new System.Drawing.Point(379, 14);
			this.label2.Margin = new System.Windows.Forms.Padding(30, 0, 0, 0);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(124, 25);
			this.label2.TabIndex = 9;
			this.label2.Text = "Remove Filter";
			// 
			// FilterTextBox
			// 
			this.FilterTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
			this.FilterTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.FilterTextBox.Location = new System.Drawing.Point(513, 15);
			this.FilterTextBox.Margin = new System.Windows.Forms.Padding(10, 0, 0, 0);
			this.FilterTextBox.Name = "FilterTextBox";
			this.FilterTextBox.Size = new System.Drawing.Size(400, 24);
			this.FilterTextBox.TabIndex = 10;
			this.FilterTextBox.Text = "OnMouseMove";
			// 
			// SaveButton
			// 
			this.SaveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
			this.SaveButton.AutoSize = true;
			this.SaveButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.SaveButton.Location = new System.Drawing.Point(1073, 9);
			this.SaveButton.Margin = new System.Windows.Forms.Padding(10, 0, 0, 0);
			this.SaveButton.Name = "SaveButton";
			this.SaveButton.Size = new System.Drawing.Size(75, 35);
			this.SaveButton.TabIndex = 12;
			this.SaveButton.Text = "Save";
			this.SaveButton.UseVisualStyleBackColor = true;
			// 
			// ServerForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(144F, 144F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
			this.ClientSize = new System.Drawing.Size(1350, 1144);
			this.Controls.Add(this.tableLayoutPanel1);
			this.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
			this.Name = "ServerForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Tracer Server";
			this.tableLayoutPanel1.ResumeLayout(false);
			this.tableLayoutPanel1.PerformLayout();
			this.tableLayoutPanel2.ResumeLayout(false);
			this.tableLayoutPanel2.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
		private System.Windows.Forms.TextBox ReceivedTextBox;
		private System.Windows.Forms.CheckBox FileModeCheckBox;
		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
		private System.Windows.Forms.TextBox AddressTextBox;
		private System.Windows.Forms.Label Label1;
		private System.Windows.Forms.TextBox FilterTextBox;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button ListenButton;
		private System.Windows.Forms.Button SaveButton;
	}
}

