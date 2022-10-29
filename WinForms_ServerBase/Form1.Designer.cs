namespace WinForms_ServerBase
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.cboAudioDevice = new System.Windows.Forms.ComboBox();
            this.pbVolume = new System.Windows.Forms.ProgressBar();
            this.SuspendLayout();
            // 
            // cboAudioDevice
            // 
            this.cboAudioDevice.FormattingEnabled = true;
            this.cboAudioDevice.Location = new System.Drawing.Point(12, 12);
            this.cboAudioDevice.Name = "cboAudioDevice";
            this.cboAudioDevice.Size = new System.Drawing.Size(198, 23);
            this.cboAudioDevice.TabIndex = 6;
            // 
            // pbVolume
            // 
            this.pbVolume.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pbVolume.Location = new System.Drawing.Point(216, 12);
            this.pbVolume.Name = "pbVolume";
            this.pbVolume.Size = new System.Drawing.Size(323, 23);
            this.pbVolume.TabIndex = 5;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.cboAudioDevice);
            this.Controls.Add(this.pbVolume);
            this.Name = "Form1";
            this.Text = "Form1";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private ComboBox cboAudioDevice;
        private ProgressBar pbVolume;
    }
}