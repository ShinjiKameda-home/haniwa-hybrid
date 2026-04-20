from multiprocessing import shared_memory

try:
    shm = shared_memory.SharedMemory(name="garden_person_presence")
    val = bool(shm.buf[0])
    print(f"成功！共有メモリの値は: {val}")
    shm.close()
except FileNotFoundError:
    print("エラー..共有メモリが見つかりません。Weatherサービスが作ってないかもしれません。")
except PermissionError:
    print("エラー..権限がありません。User/Groupの設定がまだ不十分です。")
except Exception as e:
    print(f"想定外のエラー: {e}")