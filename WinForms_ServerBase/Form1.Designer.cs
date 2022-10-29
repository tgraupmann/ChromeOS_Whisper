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
            this.components = new System.ComponentModel.Container();
            this.cboAudioDevice = new System.Windows.Forms.ComboBox();
            this.pbVolume = new System.Windows.Forms.ProgressBar();
            this.txtTranslations = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // cboAudioDevice
            // 
            this.cboAudioDevice.FormattingEnabled = true;
            this.cboAudioDevice.Location = new System.Drawing.Point(12, 12);
            this.cboAudioDevice.Name = "cboAudioDevice";
            this.cboAudioDevice.Size = new System.Drawing.Size(367, 23);
            this.cboAudioDevice.TabIndex = 6;
            this.cboAudioDevice.SelectedIndexChanged += new System.EventHandler(this.cboAudioDevice_SelectedIndexChanged);
            // 
            // pbVolume
            // 
            this.pbVolume.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pbVolume.Location = new System.Drawing.Point(385, 12);
            this.pbVolume.Name = "pbVolume";
            this.pbVolume.Size = new System.Drawing.Size(323, 23);
            this.pbVolume.TabIndex = 5;
            // 
            // txtTranslations
            // 
            this.txtTranslations.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtTranslations.Font = new System.Drawing.Font("Calibri", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.txtTranslations.Location = new System.Drawing.Point(12, 74);
            this.txtTranslations.Multiline = true;
            this.txtTranslations.Name = "txtTranslations";
            this.txtTranslations.Size = new System.Drawing.Size(776, 364);
            this.txtTranslations.TabIndex = 7;
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Calibri", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.label1.Location = new System.Drawing.Point(13, 45);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(92, 19);
            this.label1.TabIndex = 8;
            this.label1.Text = "Translations:";
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtTranslations);
            this.Controls.Add(this.cboAudioDevice);
            this.Controls.Add(this.pbVolume);
            this.Name = "Form1";
            this.Text = "Whisper AI Translations";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private ComboBox cboAudioDevice;
        private ProgressBar pbVolume;
        private TextBox txtTranslations;
        private Label label1;
        private System.Windows.Forms.Timer timer1;
    }
}