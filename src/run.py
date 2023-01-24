from multiprocessing import Process
import os
import time


def bcjh(dir):
    os.system(f"stdbuf -oL ./bcjh -s > {dir} && echo '已写入{dir}'")


if __name__ == '__main__':
    a = time.time()
    core = int(os.popen("nproc").read())
    process_list = []
    print("请稍候。")
    for i in range(core):
        p = Process(target=bcjh, args=(f'{i}.txt',))
        p.start()
        process_list.append(p)
        time.sleep(1)
    j = 0
    for i in process_list:
        p.join()
        # print(f"结束{j}")
        j += 1

    for i in range(core):
        with open(f"{i}.txt", "r") as f:
            print(i)
            print(f.read())

    print(f'共得到{core}组方案。')
    print(f'耗时{time.time()-a}秒。')
