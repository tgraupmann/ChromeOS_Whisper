# install dependencies:
# sudo apt install uvicorn
# pip3 install FastAPI[all]
# pip3 install uvloop

# launch the app
# python -m uvicorn Server_base:app --reload
# browse: http://127.0.0.1:8000

from fastapi import FastAPI
from fastapi.responses import HTMLResponse

def readServerHtml():
  htmlFile = open('Server.html', mode='r') 
  htmlContent = htmlFile.read() 
  htmlFile.close()
  return htmlContent

app = FastAPI()

@app.get('/api')
async def index_page():
  return {"hello":"world"}

@app.get("/", response_class=HTMLResponse)
async def read_items():
  return HTMLResponse(content=readServerHtml(), status_code=200)
