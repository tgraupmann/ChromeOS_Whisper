import whisper

print("Loading Whisper Model...")

#model = whisper.load_model("tiny")
#model = whisper.load_model("base") #max size supported by Samsung ChromeBook4 with 4GB of RAM
#model = whisper.load_model("small")
model = whisper.load_model("medium")
#model = whisper.load_model("large") # too large for my PC

for i in range(0, 5):

  print(i, "Transcribe Test_Chinese.mp3 in progress...")

  results = model.transcribe("Test_Chinese.mp3", language="en")

  print(i, "Transcribe Test_Chinese.mp3 complete!")

  print(i, "Transcribe results: ", results["text"])
