using NAudio.CoreAudioApi;
using NAudio.Wave;
using Newtonsoft.Json.Linq;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Net.Http.Json;
using System.Security.Policy;
using System.Text;

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

        //private static readonly HttpClient client = new HttpClient();

        public Form1()
        {
            InitializeComponent();
            AudioValues = new double[SampleRate * BufferMilliseconds / 1000];
        }

        private async void Translate()
        {
            try
            {
                System.Net.ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls | SecurityProtocolType.Tls11 | SecurityProtocolType.Tls12;
                System.Net.ServicePointManager.Expect100Continue = false;

                HttpClient client = new HttpClient();

                var httpRequestMessage = new HttpRequestMessage(HttpMethod.Post, "http://localhost:8000/translate");

                JObject jobject = new JObject();
                jobject["sampleRate"] = SampleRate;

                JArray jarray = new JArray();
                for (int i = 0; i < AudioValues.Length; i++)
                {
                    jarray.Add(AudioValues[i] / 32767.0f);
                }

                jobject["data"] = jarray;

                string pJsonContent = jobject.ToString();

                HttpContent httpContent = new StringContent(pJsonContent, Encoding.UTF8, "application/json");
                httpRequestMessage.Content = httpContent;
                var productValue = new ProductInfoHeaderValue("ScraperBot", "1.0");
                var commentValue = new ProductInfoHeaderValue("(+http://www.API.com/ScraperBot.html)");
                //httpRequestMessage.Headers.Accept.Add(new MediaTypeWithQualityHeaderValue("application/json"));
                httpRequestMessage.Headers.UserAgent.Add(productValue);
                httpRequestMessage.Headers.UserAgent.Add(commentValue);

                var response = await client.SendAsync(httpRequestMessage);
                if (response != null)
                {
                    using (HttpContent content = response.Content)
                    {
                        string result = await content.ReadAsStringAsync();
                        BeginInvoke(() =>
                        {
                            txtTranslations.Text = result;
                        });
                    }
                }
            }
            catch (Exception ex)
            {
                if (ex != null)
                {

                }
            }
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

        DateTime _mTimerSend = DateTime.MinValue;

        void Wave_DataAvailable(object? sender, NAudio.Wave.WaveInEventArgs e)
        {
            for (int i = 0; i < e.Buffer.Length / 2; i++)
            {
                if (i < AudioValues.Length)
                {
                    AudioValues[i] = BitConverter.ToInt16(e.Buffer, i * 2);
                }
            }

            if (_mTimerSend < DateTime.Now)
            {
                _mTimerSend = DateTime.Now + TimeSpan.FromSeconds(1);
                Translate();
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