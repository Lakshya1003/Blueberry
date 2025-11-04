import pvporcupine
import pyaudio
from config import ASSISTANT_NAME


# ==============================

import pyttsx3
import speech_recognition as sr
import eel
import time
import struct
import playsound

#=================================================================

HotWord = pvporcupine.create(keywords=[ASSISTANT_NAME])


#=================================================================

def hotword():
    porcupine = None
    paud = None
    audio_stream = None

    try:
        porcupine = HotWord
        paud = pyaudio.PyAudio()
        audio_stream = paud.open(
            rate=porcupine.sample_rate,
            channels=1,
            format=pyaudio.paInt16,
            input=True,
            frames_per_buffer=porcupine.frame_length)
        print(f"Listening for hotword '{ASSISTANT_NAME}'...")
        while True:
            keyword = audio_stream.read(porcupine.frame_length)
            keyword = struct.unpack_from("h" * porcupine.frame_length, keyword)
            if porcupine.process(keyword) >= 0:
                print("hotword detected")
    except Exception as e:
        print(f"cant hear anything: {e}")
    finally:
        if audio_stream is not None:
            audio_stream.close()
        if paud is not None:
            paud.terminate()
        if porcupine is not None:
            porcupine.delete()

if __name__ == "__main__":
    playsound.playsound("assets/audio/start.mp3")
    hotword()