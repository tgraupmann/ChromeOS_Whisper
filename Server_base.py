# install dependencies:
# sudo apt install uvicorn
# pip3 install FastAPI[all]
# pip3 install uvloop

# launch the app
# python -m uvicorn Server_base:app --reload
# browse: http://127.0.0.1:8000

from fastapi import FastAPI
from fastapi.responses import HTMLResponse
from pydantic import BaseModel
from typing import List
import whisper

print("Loading Whisper Model...")

model = whisper.load_model("tiny")
#model = whisper.load_model("base") #max size supported by Samsung ChromeBook4 with 4GB of RAM
#model = whisper.load_model("small")
#model = whisper.load_model("medium")
#model = whisper.load_model("large") # too large for my PC

def readServerHtml():
  htmlFile = open('Server.html', mode='r') 
  htmlContent = htmlFile.read() 
  htmlFile.close()
  return htmlContent

app = FastAPI()

class TranslateItem(BaseModel):
  data: List[float]

@app.post('/translate')
async def api_translate(item:TranslateItem):
  print("Transcription: Start...")
  results = model.transcribe("Test_MP3.mp3", language="en")
  #results = model.transcribe([1,2,3], language="en")
  waveData = item.data;
  #print("waveData", waveData)
  #results = model.transcribe(waveData, language="en")
  print("Transcription: Done!")
  return {"text":results["text"]}

@app.get("/", response_class=HTMLResponse)
async def read_items():
  return HTMLResponse(content=readServerHtml(), status_code=200)

print("Whisper Server loaded!")
