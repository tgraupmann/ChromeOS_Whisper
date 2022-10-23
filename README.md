# ChromeOS_Whisper
Run Whisper AI Model on ChromeOS

Change Log:

Initial commit of project

Process:

* Setup Chromebook in developer mode (this wipes all data) - https://chromeready.com/1186/how-to-enable-developer-mode-on-a-chromebook/

* Setup Linux Subsystem with 12GB of space. - https://support.google.com/chromebook/answer/9145439

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
rm -r -f ~/tmp
```

* Update the symbolic links for Python.

```
sudo rm /usr/bin/python
sudo rm /usr/bin/pip
sudo ln -s ~/anaconda3/bin/python /usr/bin/python
sudo ln -s ~/anaconda3/bin/pip /usr/bin/pip
```

* Install Visual Studio Code - https://chromeunboxed.com/how-to-install-visual-studio-code-on-a-chromebook/

* Open the [Whisper Notebook](Whisper.ipynb) in VS Code

* Record new audio clips with a microphone recorder tool like `Voice Recorder` which saves `WAV` and `MP3` files. (MP3 runs faster with Whisper). Recordings save to `Play Files\Recorders`.
