# О системе
Система предназначена для работы с базой данных сотрудников компании.

Она позволяет отслеживать данные о сотрудниках и управлять ими (если вы являетесь *Директором*/*Бухгалтером*), а *Сотрудник* может узнать свои данные и начальника своего отдела/своих подчинённых, если он *Начальник Отдела*.
___
## Пользователи системы и их роли
- *Директор*  – может добавлять и удалять сотрудников, просматривать* и редактировать все их данные** (кроме зарплаты), редактировать свой ключ доступа
- *Бухгалтер* – может просматривать все данные сотрудников (кроме ключа доступа), редактировать их зарплату, редактировать свой ключ доступа
- *Сотрудник* – может просматривать свои данные и ФИО начальника своего отдела, редактировать свой ключ доступа
- *Начальник Отдела* – может просматривать свои данные и данные подчинённых своего отдела, редактировать свой ключ доступа
###### * Под "просмотром данных" подрузамевается формирование отчёта о доступных данных.
###### ** Данные включают в себя ФИО, отдел, должность, размер зарплаты, является ли сотрудник начальником отдела, ключ доступа.
___
## Список основных функций
Просмотр данных о сотруднике/сотрудниках, их редактирование и добавление новых, система авторизации с помощью ключей доступа.
___
## Use case диаграмма
[![ucd](https://img.plantuml.biz/plantuml/svg/fLJDQjj04BuBz0v3BcqFUOMIbgJGGmYqJx386yTKaOAqFeHIS6n23WajXXvwQTf27a1r8whQoUOLPj-eCsl9McrIafAJvVdvlc--sTsDN8bC3VVZC8ZbhWALGZRexode3J8PgK6QX44OY4Yb6QpX3zHcR4PcWfTuZHNErq3aiBL_4AU7KhPrNx4m9rZWmfmjwm3FpPYQIbnWOPksHIBwCcjxFcDbHbGmfq9jHhPequf8dQgfcPXZ_6MnnrZMTI-YA1qcIYIArNAFIFgn119ScICkn2iiu7qO00np6ObS4jOtujFcY59sAY1z1SczDgVc6503fKhyYQMbo6MysnDAT26-KmLtiMYMht5cCIeikNr6WPBKQi0v2Vx8fZ2DPb7auarjnZjvkDKRACQ-H-85Vi5p80RFOCBjycdFxrUlTn8HxTsck62tYS2NRnbFp9Hm2dO3P-m7E-FQL9DM7B-XZqkILL5ksbGibvtqtyY3DBjZmiVHFoNdkU-AEngBI_pZcVoiRQSVxmmpRNsA4jNAl6V9X746-9i-HiImi849i08wCjhku0wgvIxUYbXa_zZuu_nuo9Wlqpp_pp7jLEQKPQpO-C-3fcM-JRevpRmqy0U-fa_uUixC9pAnWffNBta9Z8K1Amk3-gc0zVNdpZLqeyxvSiEhQnK6pHlbWNJ2golIYJeWxJFZmNW9zo1vANVrNieXQjuz6tVtrKcq_BVgMxyxAOTYGoOzUlR_0W00)](https://editor.plantuml.com/uml/fLJDQjj04BuBz0v3BcqFUOMIbgJGGmYqJx386yTKaOAqFeHIS6n23WajXXvwQTf27a1r8whQoUOLPj-eCsl9McrIafAJvVdvlc--sTsDN8bC3VVZC8ZbhWALGZRexode3J8PgK6QX44OY4Yb6QpX3zHcR4PcWfTuZHNErq3aiBL_4AU7KhPrNx4m9rZWmfmjwm3FpPYQIbnWOPksHIBwCcjxFcDbHbGmfq9jHhPequf8dQgfcPXZ_6MnnrZMTI-YA1qcIYIArNAFIFgn119ScICkn2iiu7qO00np6ObS4jOtujFcY59sAY1z1SczDgVc6503fKhyYQMbo6MysnDAT26-KmLtiMYMht5cCIeikNr6WPBKQi0v2Vx8fZ2DPb7auarjnZjvkDKRACQ-H-85Vi5p80RFOCBjycdFxrUlTn8HxTsck62tYS2NRnbFp9Hm2dO3P-m7E-FQL9DM7B-XZqkILL5ksbGibvtqtyY3DBjZmiVHFoNdkU-AEngBI_pZcVoiRQSVxmmpRNsA4jNAl6V9X746-9i-HiImi849i08wCjhku0wgvIxUYbXa_zZuu_nuo9Wlqpp_pp7jLEQKPQpO-C-3fcM-JRevpRmqy0U-fa_uUixC9pAnWffNBta9Z8K1Amk3-gc0zVNdpZLqeyxvSiEhQnK6pHlbWNJ2golIYJeWxJFZmNW9zo1vANVrNieXQjuz6tVtrKcq_BVgMxyxAOTYGoOzUlR_0W00)
___
## Блок схемы
![P A](PA.png)
___
***Проект реализован на языке C++ с использованием платформы Visual Studio и библиотеки SQLite***
