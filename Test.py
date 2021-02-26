# import datetime
# import time
# import webbrowser
# import os
#
# target_time = datetime.datetime(2021, 2, 12, 11, 50)
#
# while datetime.datetime.now() < target_time:
#     time.sleep(10)
# print('It is 8:45 am, now running the rest of the code')
# url = "https://us04web.zoom.us/j/74092507701?pwd=TlcxYmJOUjU0aDc1TmxVQUJ6N3FDZz09"
# webbrowser.get("C:/Program Files (x86)/Google/Chrome/Application/chrome.exe %s").open(url)
#
# target_time = datetime.datetime(2021, 2, 12, 11, 51)
# while datetime.datetime.now() < target_time:
#     time.sleep(10)
#
# os.system("TASKKILL /F /IM zoom.exe")


size = 0

with open("test.txt", 'w')as file:
    for number in range(1000000, 10000000):
        digits = []
        number = str(number)
        for chr in number:
            digits.append(chr)
        digits = list(dict.fromkeys(digits))  # Removing duplicates
        if len(digits) == 3:
            print(number)
            file.write(number + "\n")
            size += 1
    print(size)
