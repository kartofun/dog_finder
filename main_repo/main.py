import lib.just_library as l

from typing import Union
from fastapi import FastAPI, Response, File, UploadFile
from fastapi.responses import JSONResponse
from pydantic import BaseModel

tags_metadata = [
    {
        "name": "find_dog",
        "description": "Распознает собаку на загруженной картинке. ",
    }
]

app = FastAPI(openapi_tags=tags_metadata)

max_size=10485760

@app.get("/")
def read_root():
    return {"Welcom to the dog_finder server. Try 'find_dog' in Swagger docs: http://0.0.0.0:8000/docs"}

@app.post("/file/find-dog", status_code=200, tags=["find_dog"])
def find_dog(response: Response,file_bytes: bytes = File()):
    """
        Метод ищет на загруженной картинке собаку, и в случае успеха возвращает 'found_dog': True
    """
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