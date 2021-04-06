import matplotlib.pyplot as plt


def main():
        i = 0
        times_w0_serial = []
        times_w0s = []
        times_w0w = []
        try:
            f = open("results.txt", "r")
        except IOError:
            print("File does not exist")
            exit(0)
        while i <= 2:
            line = f.readline()
            split = line.split()
            print(split)
            if len(split) == 0:
                i += 1
            for j in range(len(split)):
                split[j] = split[j].replace(",", "")
            try:
                if split[0] == "w0" and i == 0:
                    num = float(split[2])
                    times_w0_serial.append(num)
                elif split[0] == "w0" and i == 1:
                    num = float(split[2])
                    times_w0s.append(num)
                elif split[0] == "w0" and i == 2:
                    num = float(split[2])
                    times_w0w.append(num)
            except:
                continue

        items = [100, 200, 300, 400, 500, 600, 700, 800, 900, 1000]
        np = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
        np_items = ["2/200", "3/300", "4/400", "5/500", "6/600", "7/700", "8/800", "9/900",
                    "10/1000", "11/1100", "12/1200", "13/1300", "14/1400", "15/1500", "16/1600"]
        # serial, w0
        plt.plot(items, times_w0_serial)

        plt.xlabel("Items")
        plt.ylabel("time (in sec)")
        plt.title("Serial; p = w0")

        plt.show()

        # parallel, strong-scaling, w0
        plt.plot(np, times_w0s)

        plt.xlabel("np")
        plt.ylabel("time (in sec)")
        plt.title("Strong Scaling; platform = w0, items = 1000")

        plt.show()

        # parallel, weak-scaling, w0
        plt.plot(np_items, times_w0w)

        plt.xlabel("np/Items")
        plt.ylabel("time (in sec)")
        plt.xticks(["2/200", "4/400", "6/600", "8/800", "10/1000", "12/1200", "14/1400", "16/1600"])
        plt.title("Weak Scaling; platform = w0")

        plt.show()


if __name__ == "__main__":
        main()
