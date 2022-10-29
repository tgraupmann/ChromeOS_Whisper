using NAudio.CoreAudioApi;
using NAudio.Wave;
using NAudio.Wave.SampleProviders;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.Net;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Net.Http.Json;
using System.Security.Policy;
using System.Text;
using System.Threading.Channels;

namespace WinForms_ServerBase
{
    public partial class Form1 : Form
    {
        NAudio.Wave.WaveInEvent? Wave;
        NAudio.Wave.WasapiLoopbackCapture? CaptureDevice = null;

        readonly List<int> AudioValues = new List<int>();

        const int DEFAULT_SAMPLE_RATE = 16000;
        const int DEFAULT_CHANNEL_COUNT = 1;

        private int SampleRate = DEFAULT_SAMPLE_RATE;
        private int ChannelCount = DEFAULT_CHANNEL_COUNT;
        readonly int BitDepth = 16;
        readonly int BufferMilliseconds = 1000;

        private int Volume = 0;

        private static readonly HttpClient client = new HttpClient();

        public Form1()
        {
            InitializeComponent();
        }

        private async void Translate()
        {
            try
            {
                /*
                // Initially the WSL2 guest was not allowing host traffic
                System.Net.ServicePointManager.SecurityProtocol =
                    SecurityProtocolType.Tls | SecurityProtocolType.Tls11 | SecurityProtocolType.Tls12;
                System.Net.ServicePointManager.Expect100Continue = false;
                */

                HttpRequestMessage httpRequestMessage =
                    new HttpRequestMessage(HttpMethod.Post, "http://localhost:8000/translate");

                JObject jobject = new JObject();
                jobject["sampleRate"] = SampleRate;

                JArray jarray = new JArray();
                for (int i = 0; i < AudioValues.Count; i++)
                {
                    float data = (float)(AudioValues[i] / 32767.0f);
                    jarray.Add(data);
                }

                jobject["data"] = jarray;

                AudioValues.Clear();

                string pJsonContent = jobject.ToString();

                HttpContent httpContent = new StringContent(pJsonContent, Encoding.UTF8, "application/json");
                httpRequestMessage.Content = httpContent;
                
                var productValue = new ProductInfoHeaderValue("Whisper_Client", "1.0");
                var commentValue = new ProductInfoHeaderValue("(+http://localhost:8000/transcribe)");
                httpRequestMessage.Headers.UserAgent.Add(productValue);
                httpRequestMessage.Headers.UserAgent.Add(commentValue);

                var response = await client.SendAsync(httpRequestMessage);
                if (response != null)
                {
                    using (HttpContent content = response.Content)
                    {
                        string text = await content.ReadAsStringAsync();
                        BeginInvoke(() =>
                        {
                            try
                            {
                                JObject jobject = JObject.Parse(text);
                                txtTranslations.Text += " " + jobject["text"];
                            }
                            catch
                            {

                            }
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
            }

            if (cboAudioDevice.SelectedIndex == -1)
                return;

            if (cboAudioDevice.SelectedIndex < NAudio.Wave.WaveIn.DeviceCount)
            {
                SampleRate = DEFAULT_SAMPLE_RATE;
                ChannelCount = DEFAULT_CHANNEL_COUNT;

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
                foreach (MMDevice? wasapi in enumerator.EnumerateAudioEndPoints(DataFlow.Render, DeviceState.Active))
                {
                    if (cboAudioDevice.SelectedIndex == selectedIndex)
                    {
                        //SampleRate = wasapi.AudioClient.MixFormat.SampleRate;
                        //ChannelCount = wasapi.AudioClient.MixFormat.Channels;

                        MMDevice loopbackCaptureDevice = wasapi;
                        CaptureDevice = new WasapiLoopbackCapture(loopbackCaptureDevice);
                        CaptureDevice.WaveFormat = new WaveFormat(16000, 16, 1);
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
            if (e.BytesRecorded == 0)
            {
                return;
            }

            int bufferLength = e.BytesRecorded / 2;

            List<int> tempSample = new List<int>();
            for (int i = 0; i < bufferLength; i += 2)
            {
                int data = BitConverter.ToInt16(e.Buffer, i);
                tempSample.Add(data);
            }

            float volume = tempSample.Max() / 32767.0f;

            if (AudioValues.Count == 0 &&
                volume < 0.02f)
            {
                // trim noise
                return;
            }

            for (int i = 0; i < tempSample.Count; ++i)
            {
                int data = tempSample[i];
                AudioValues.Add(data);
            }

            if (AudioValues.Count > DEFAULT_SAMPLE_RATE)
            {
                if (volume > 0.02f && AudioValues.Count < (5 * DEFAULT_SAMPLE_RATE))
                {
                    // wait to translate while talking for a while
                    return;
                }
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