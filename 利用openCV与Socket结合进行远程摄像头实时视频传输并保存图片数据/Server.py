import socket
import struct
import threading
import cv2
import numpy
import os


class Server:
    def __init__(self):
        # 设置tcp服务端的socket
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # 设置重复使用
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, True)
        # 绑定地址和端口
        self.server.bind(('192.168.1.109', 11000))
        # 设置被动监听
        self.server.listen(128)

    def run(self):
        while True:
            print('等待客户端连接')
            # 等待客户端连接
            client, addr = self.server.accept()
            ProcessClient(client).start()


class ProcessClient(threading.Thread):

    def __init__(self, client):
        super().__init__()
        self.client = client

        # 以下内容作用是根据文件夹的图片序号规定'i'的初值，方便下一次保存图片时使用
        self.i = 0
        path = "D:/image"
        for root, dirs, files in os.walk(path, topdown=False):
            print(root)
            print(dirs)
            print(files)

        file_list = []
        for file in files:
            file = int(file.split('.')[0])
            file_list.append(file)

        if len(file_list) == 0:
            self.i = 0
        else:
            file_list.sort()
            self.i = file_list[-1] + 1

    def run(self):
        while True:
            data = self.client.recv(8)
            if not data:
                break
            # 图片的长度 图片的宽高
            length, width, height = struct.unpack('ihh', data)

            imgg = b''  # 存放最终的图片数据
            while length:
                # 接收图片
                temp_size = self.client.recv(length)
                length -= len(temp_size)  # 每次减去收到的数据大小
                imgg += temp_size  # 每次收到的数据存到img里

            # 把二进制数据还原
            data = numpy.fromstring(imgg, dtype='uint8')

            # 还原成矩阵数据
            image = cv2.imdecode(data, cv2.IMREAD_UNCHANGED)
            print(image)

            cv2.imshow('capture', image)
            # 保存图片
            k = cv2.waitKey(1)
            if k == ord('k'):
                cv2.imwrite(r"D:\image\\" + str(self.i) + ".jpg", image)  # 存储路径
                self.i = self.i + 1
                print(self.i)
                # time.sleep(5)

            if k == ord('q'):
                break


if __name__ == '__main__':
    server = Server()
    server.run()
