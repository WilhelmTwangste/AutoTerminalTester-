#include <Servo.h>
#include <stdlib.h>
#include <string.h>
#include "StringSplitter.h"
Servo servoX;
Servo servoY;
Servo servoZ;

void setup()
{
    servoX.attach(9); // X
    servoY.attach(10); // Y
    servoZ.attach(11); // head, static

    servoX.write(108); //право-лево
    servoY.write(72); //нажатие
    servoZ.write(10); //вверх-низ

    delay(500);
    Serial.begin(9600);
}

int term_x[5][4] = {
    { 108, 99, 90, 80}, //F1, F2, F3, F4
  { 107, 99, 89, 79}, //1, 2, 3, выкл
  { 107, 99, 89, 79}, //4, 5, 6, красная
  { 110, 100, 87, 74}, //7, 8, 9, желтая
  { 113, 100, 87, 74} //*, 0, #, зеленая
};
int term_y[5][4] = {
    { 74, 74, 74, 74}, //F1, F2, F3, F4
  { 79, 79, 79, 79}, //1, 2, 3, выкл
  { 89, 89, 89, 89}, //4, 5, 6, красная
  { 96, 96, 96, 96}, //7, 8, 9, желтая
  { 104, 105, 104, 104} //*, 0, #, зеленая
};

void serv_click(int i, int j)
{
    servoX.write(term_x[i][j]);
    delay(500);
    servoY.write(term_y[i][j]);
    delay(500);
    for (int k = 10; k >= 0; k -= 1)
    {
        servoZ.write(k);
        delay(40);
    }
    delay(500);
    for (int k = 0; k <= 10; k += 1)
    {
        servoZ.write(k);
        delay(40);
    }
}

void serv_move(int i, int j)
{
    delay(500);
    for (int k = 0; k <= 10; k += 1)
    {
        servoZ.write(k);
        delay(40);
    }
    servoZ.write(10);
    servoX.write(term_x[i][j]);
    servoY.write(term_y[i][j]);
}

void click_sum(String sum)
{
    for (int i = 0; i < sum.length(); i++)
    {
        if (sum[i] == '1') serv_click(1, 0);
        if (sum[i] == '2') serv_click(1, 1);
        if (sum[i] == '3') serv_click(1, 2);
        if (sum[i] == '4') serv_click(2, 0);
        if (sum[i] == '5') serv_click(2, 1);
        if (sum[i] == '6') serv_click(2, 2);
        if (sum[i] == '7') serv_click(3, 0);
        if (sum[i] == '8') serv_click(3, 1);
        if (sum[i] == '9') serv_click(3, 2);
        if (sum[i] == '0') serv_click(4, 1);
    }
}

void operations(int operation, int position, int check, String sum_pur, String sum_re)
{
    serv_click(4, 1);
    if (position > 0)
    {
        for (int k = 0; k < position; k++)
        {
            serv_click(0, 2);
        }
    }
    if (operation == 1)
    { //оплата
        serv_click(4, 3);
        click_sum(sum_pur);
        serv_click(4, 3);
    }
    if (operation == 3)
    { //отмена
        serv_click(4, 3);
        click_sum(String(check));
        serv_click(4, 3);
        if (sum_re == "0")
        {
            serv_click(4, 3);
        }
        else
        {
            int i = 0;
            while (i != sum_pur.length())
            {
                serv_click(3, 3);
                i += 1;
            }
            click_sum(sum_re);
            serv_click(4, 3);
        }
    }
    if (operation == 2)
    { //возврат
        serv_click(4, 3);
        click_sum(sum_pur);
        serv_click(4, 3);
        serv_click(4, 3);
    }
    Serial.flush();
}

void close_batch()
{
    serv_click(1, 2);
    serv_click(1, 0);
}

void loop()
{
    String com, oper, pos, sum_re, sum_pur, checks_count;
    if (Serial.available())
    {
        com = Serial.readString();
        StringSplitter* splitter = new StringSplitter(com, '-', 5);
        int itemCount = splitter->getItemCount();

        for (int i = 0; i < itemCount; i++)
        {
            String item = splitter->getItemAtIndex(i);
            if (i == 0)
            {
                oper = item;
            }
            if (i == 1)
            {
                pos = item;
            }
            if (i == 2)
            {
                checks_count = item;
            }
            if (i == 3)
            {
                sum_pur = item;
            }
            if (i == 4)
            {
                sum_re = item;
            }
        }

        String operation_string, position_string, operation_sub;
        int checks_len = checks_count.toInt();
        int checks_pur[checks_len];
        int k = 0;
        int ch_num = 1;
        for (int i = 0; i < oper.length(); i++)
        {
            if (oper[i] == '1')
            {
                checks_pur[k] = ch_num;
                k++;
            }
            ch_num++;
        }


        int j = 0;
        for (int i = 0; i < oper.length(); i++)
        {
            operation_string = String(oper[i]);
            position_string = String(pos[i]);
            if (oper[i] == '3' || oper[i] == '2')
            {
                operations(operation_string.toInt(), position_string.toInt(), checks_pur[j], sum_pur, sum_re);
                j++;
            }
            ///оделать
            // else operations(operation_string.toInt(), position_string.toInt(), checks_pur[j], sum_pur, sum_re);

            else if (oper[i] == '4')
            {
                close_batch();
                operation_sub = operation_string.substring(i, operation_string.length());
                k = 0;
                ch_num = 1;
                for (int i = 0; i < operation_sub.length(); i++)
                {
                    if (operation_sub[i] == '1')
                    {
                        checks_pur[k] = ch_num;
                        k++;
                    }
                    ch_num++;
                }
                j = 0;
            }
            else operations(operation_string.toInt(), position_string.toInt(), checks_pur[0], sum_pur, sum_re);

            delay(10000);
        }
    }
}
