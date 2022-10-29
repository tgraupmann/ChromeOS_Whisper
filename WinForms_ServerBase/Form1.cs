using NAudio.CoreAudioApi;
using NAudio.Wave;

namespace WinForms_ServerBase
{
    public partial class Form1 : Form
    {
        NAudio.Wave.WaveInEvent? Wave;
        NAudio.Wave.WasapiLoopbackCapture? CaptureDevice = null;

        readonly double[] AudioValues;

        readonly int SampleRate = 16000;
        readonly int BitDepth = 16;
        readonly int ChannelCount = 1;
        readonly int BufferMilliseconds = 1000;

        public Form1()
        {
            InitializeComponent();
            AudioValues = new double[SampleRate * BufferMilliseconds / 1000];
        }

        private void StopOutputDeviceRecording()
        {
            if (CaptureDevice is not null)
            {
                CaptureDevice.StopRecording();
                CaptureDevice.Dispose();
                CaptureDevice = null;

                for (int i = 0; i < AudioValues.Length; i++)
                {
                    AudioValues[i] = 0;
                }
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            #region Input devices

            for (int i = 0; i < NAudio.Wave.WaveIn.DeviceCount; i++)
            {
                var caps = NAudio.Wave.WaveIn.GetCapabilities(i);
                cboAudioDevice.Items.Add(caps.ProductName);
            }

            #endregion Input devices


            #region Output devices

            var enumerator = new MMDeviceEnumerator();
            foreach (var wasapi in enumerator.EnumerateAudioEndPoints(DataFlow.Render, DeviceState.Active))
            {
                cboAudioDevice.Items.Add(wasapi.FriendlyName);
            }

            #endregion Output devices
        }

        private void cboAudioDevice_SelectedIndexChanged(object sender, EventArgs e)
        {
            StopOutputDeviceRecording();

            if (Wave is not null)
            {
                Wave.StopRecording();
                Wave.Dispose();

                for (int i = 0; i < AudioValues.Length; i++)
                {
                    AudioValues[i] = 0;
                }
            }

            if (cboAudioDevice.SelectedIndex == -1)
                return;

            if (cboAudioDevice.SelectedIndex < NAudio.Wave.WaveIn.DeviceCount)
            {
                Wave = new NAudio.Wave.WaveInEvent()
                {
                    DeviceNumber = cboAudioDevice.SelectedIndex,
                    WaveFormat = new NAudio.Wave.WaveFormat(SampleRate, BitDepth, ChannelCount),
                    BufferMilliseconds = BufferMilliseconds
                };

                Wave.DataAvailable += Wave_DataAvailable;
                Wave.StartRecording();
            }
            else
            {
                #region Output devices

                var enumerator = new MMDeviceEnumerator();
                int selectedIndex = NAudio.Wave.WaveIn.DeviceCount;
                foreach (var wasapi in enumerator.EnumerateAudioEndPoints(DataFlow.Render, DeviceState.Active))
                {
                    if (cboAudioDevice.SelectedIndex == selectedIndex)
                    {
                        MMDevice loopbackCaptureDevice = wasapi;
                        CaptureDevice = new WasapiLoopbackCapture(loopbackCaptureDevice);
                        CaptureDevice.DataAvailable += Wave_DataAvailable;
                        CaptureDevice.StartRecording();
                        break;
                    }
                    ++selectedIndex;
                }

                #endregion Output devices
            }
        }

        void Wave_DataAvailable(object? sender, NAudio.Wave.WaveInEventArgs e)
        {
            for (int i = 0; i < e.Buffer.Length / 2; i++)
            {
                if (i < AudioValues.Length)
                {
                    AudioValues[i] = BitConverter.ToInt16(e.Buffer, i * 2);
                }
            }
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            StopOutputDeviceRecording();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            try
            {
                int level = (int)AudioValues.Max();

                // auto-scale the maximum progressbar level
                if (level > pbVolume.Maximum)
                    pbVolume.Maximum = level;
                pbVolume.Value = level;
            }
            catch (Exception)
            {
                //handle hardware muting the mic
            }
        }
    }
}