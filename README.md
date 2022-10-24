# ChromeOS_Whisper
Run Whisper AI Model on ChromeOS

## Intro

[Whisper](https://github.com/openai/whisper) is an awesome AI model that does speech detection and transcription for multiple languages.

Change Log:

```
Initial commit of project
```

Testing:

* Using a Samsung ChromeBook 4, transcribing an English sentence from MP3 takes around 30 seconds.

* Use the `base` model on CB4 to avoid running out of memory when loading the model.

```
model = whisper.load_model("base")
```

* English language works.

* Chinese language transcribed `你好` (Hello).

* Running `whisper` with `WSL2` on `Windows` transcribes a sentence in `0.3 seconds`.

Process:

* Setup Chromebook in developer mode (this wipes all data) - https://chromeready.com/1186/how-to-enable-developer-mode-on-a-chromebook/

* Setup Linux Subsystem with 12GB of space. - https://support.google.com/chromebook/answer/9145439

* Install dependencies:

```
sudo apt-get update
sudo apt-get install curl
sudo apt-get install ffmpeg
```

* Install Anaconda - https://chromeready.com/5218/install-update-anaconda-chromebook/

* Install Anaconda through the `penguin` terminal so it shares the environment with VS Code.

```
mkdir ~/tmp && cd ~/tmp
curl -O https://repo.anaconda.com/archive/Anaconda3-2022.10-Linux-x86_64.sh
chmod +x ./Anaconda3-2022.10-Linux-x86_64.sh
./Anaconda3-2022.10-Linux-x86_64.sh
```

* Clean up the temp folder after install is complete.

```
cd ~
rm -r -f ~/tmp
```

* Update the symbolic links for Python.

```
sudo rm /usr/bin/python
sudo rm /usr/bin/pip
sudo rm /usr/bin/pip3
sudo rm /usr/bin/conda
sudo rm /usr/bin/jupiter
sudo ln -s ~/anaconda3/bin/python /usr/bin/python
sudo ln -s ~/anaconda3/bin/pip /usr/bin/pip
sudo ln -s ~/anaconda3/bin/pip3 /usr/bin/pip3
sudo ln -s ~/anaconda3/bin/conda /usr/bin/conda
sudo ln -s ~/anaconda3/bin/conda /usr/bin/jupiter
```

* Install [Visual Studio Code](https://code.visualstudio.com/download) - https://chromeunboxed.com/how-to-install-visual-studio-code-on-a-chromebook/

* Check out this repository to a place that Linux has access to like ~/Downloads/Whisper - https://www.techrepublic.com/article/how-to-share-files-with-linux-apps-on-chrome-os/

* Share the Downloads/Whisper folder with the Linux environment.

* Open the [Whisper Notebook](Whisper.ipynb) in VS Code

* Running the first cell in the notebook might prompt to install extensions for `Python + Jupiter`. Accept installing the extensions and restart `VS-Code` after the extension installation completes. This allows you to execute the code snippets in the notebook directly from `VS Code`.

![image_1](images/image_1.png)

* Running a notebook cell might prompt to `Connect to a Jupyter Server`.

![image_2](images/image_2.png)

* Record new audio clips with a microphone recorder tool like `Voice Recorder` which saves `WAV` and `MP3` files. (MP3 runs faster with Whisper). Recordings save to `Play Files\Recorders`.

## WSL2 on Windows

* Install WSL2 with Ubuntu 22.04 on Windows - https://apps.microsoft.com/store/detail/ubuntu/9PDXGNCFSCZV

* Follow the steps from above and run in the Ubuntu terminal. You should install the dependencies including Anaconda with the same code above in the Ubuntu terminal.

* Get the code and open `VS Code`

```
mkdir ~/Public
cd ~/Public
git clone https://github.com/tgraupmann/ChromeOS_Whisper
cd ChromeOS_Whisper
```

* Install dependencies to run `jupyter` server.

```
pip3 install jupyter
pip3 install jupyterlab
pip3 install markupsafe==2.0.1
pip3 install traitlets==5.1.1
pip3 install pygments==2.4.1
sudo apt-get install jupyter-core
```

* Start the `jupyter` server in the `WSL2` terminal. Starting the lab logs an authorization url that can be used in the browser or `VS Code`.

```
jupyter lab
```

* The same link can be entered into `VS Code` to connect to the `jupyter server`.

![image_3](images/image_3.png)

* After connecting to the `jupyter server` select the `Whisper.ipynb` notebook and then `VS Code` can run the notebook cells.

* If you use the browser interface, use the `jupyter lab` to open the `Whisper.ipynb` notebook which should change the URL to something like - http://localhost:8888/lab/tree/Whisper.ipynb
