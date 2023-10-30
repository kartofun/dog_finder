import lib.just_library as l

from typing import Union
from fastapi import FastAPI, Response, File, UploadFile
from fastapi.responses import JSONResponse
from pydantic import BaseModel

app = FastAPI()

max_size=10485760

@app.post("/file/find-dog", status_code=200)
def find_dog(response: Response,file_bytes: bytes = File()):
    found_dog = False 
    status = "ok"
    status_code = 200
    err_msg = ""
    try:
        l_file = len(file_bytes)
        print("size_file: ", l_file)
        if l_file > max_size:
            raise Exception
        if l_file < 1:
            raise Exception

        res = l.predict(file_bytes, len(file_bytes))
        if res == "dog":
            found_dog = True

    except Exception as e:
        err_msg = "Bad image"
        status = "error"
        response.status_code = 400
    finally:
        return {"status" : status, "found_dog": found_dog, "error": err_msg }