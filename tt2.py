from time import sleep
import json
import serial.tools.list_ports

#автоопределение
ports = serial.tools.list_ports.comports()
for port in ports:
    p = str(port)[0:4]

arduino = serial.Serial(p, 9600)

# работа с файлом для нахождения порядка операций
with open('bin.txt', 'r') as file:
    lst = file.readlines()

# список операций
for l in lst:
    if l.startswith('     TAG_OPERATION_LIST  3F27'):
        lst = lst[lst.index(l):len(lst)] # 01 00 00 00

lst2 = []
# выделение значений
for l in lst:
    if l.strip().startswith('TAG_OPERATION_ID'):
        lst2.append(l[l.find(') ') + 2:l.rfind('0') + 1])

id_list = []
# выделение из них чисел
for el in lst2:
    id_list.append(int(el[0:3])) # 1, 3, 2

#обработка тестов
with open('text.json', 'r') as file:
    test_list = json.load(file)

operation_list = {'purchase': 1, 'refund': 2, 'reversal': 3}

# ардуино1
while True:
    command = input("Выберите операцию:\n1 - оплата\n2 - возврат\n3 - отмена\n4 - закрыть смену\n5 - выбрать сценарий\n6 - добавить собственный сценарий\nВвод:")
    while command == '6':
        new_test = str(input("Введите последовательность: "))
        test_list.append(new_test)
        with open('text.json', 'w') as file:
            json.dump(test_list, file)
        print("Успешно!")
        command = input("Выберите операцию:\n1 - оплата\n2 - возврат\n3 - отмена\n4 - закрыть смену\n5 - выбрать сценарий\n6 - добавить собственный сценарий\nВвод:")

    if command == '5':
        i = 0
        for el in test_list:
            print(f'{i} - {el}')
            i += 1
        test_num = int(input("Введите номер сценария: "))
        command = test_list[test_num]

    repeat_count = int(input("Введите количество повторений: "))
    if repeat_count > 0:
        command = command * repeat_count
    close = input("Выполинть закрытие смены в конце?(д/н): ")
    if close == 'д':
        command += '4'
    print(command)

    count_checks = len(command)

    order = ""
    for el in command:
            match int(el):
                case 1:
                    order += str(id_list.index(operation_list['purchase']))
                case 2:
                    order += str(id_list.index(operation_list['refund']))
                case 3:
                    order += str(id_list.index(operation_list['reversal']))
                case _:
                    order += str(id_list.index(operation_list['purchase']))

    sum_pur = 0
    sum_re = 0
    pin = 0
    if '1' in command:
        sum_pur = str(input("Введите сумму: "))

    if '2' in command or '3' in command:
        sum_re = input("Сумма возврата/отмены (ввести 0, если без изменений): ")

    if int(sum_pur) >= 3000:
        pin = input("Введите пин: ")

    arduino_wr = str(command) + '-' + str(order) + '-' + str(count_checks) + '-' + str(sum_pur) + '-' + str(sum_re) + '-' + str(pin)
   # arduino.write(sum.encode()
    print(arduino_wr)
    arduino.write(arduino_wr.encode())
    if pin != 0:
        sleep(30 * len(command) + 15)
    else:
        sleep(30 * len(command))
