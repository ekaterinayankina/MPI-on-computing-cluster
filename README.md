# MPI-on-computing-cluster
Решение задачи коммивояжера методом ветвей и границ для выполнения на вычислительном кластере.

Требуется исследовать время работы программы с входными данными различной размерности, для чего можно использовать первые N городов 
из списка. 
Построить график зависимости времени работы программы от размера задачи (количества городов). 

Для запуска программы необходимо соединить компьютеры по сети, затем на каждом используемом ПК:
1.	Создать учетные записи с одинаковым именем и паролем
2.	Поместить программу в каталоги с одинаковыми путями
3.	Включить сетевое обнаружение
4.	Выключить контроль учетных записей
5.	Выключить брандмауэр Windows (или добавить исключения) 
6.	Запустить smpd в командной строке
Затем выполнить mpiexec на одном из компьютеров.
Команда запуска программы:
>>mpiexec – d 3 – hosts 2 CALCULATOR-2016 4 DESKTOP-1PEPL04 4 MSI_TSP.exe
Параметры команды: 
d 3 – режим отладки (сообщения соединения выводятся в командную строку)
hosts 2 – запуск программы на двух ПК
CALCULATOR-2016 4 – название первого ПК и количество задействованных процессов (4)
DESKTOP-1PEPL04 4 –название второго ПК и количество задействованных процессов (4)
MSI_TSP.exe – исполняемый файл программы


Описание программы:

У каждого процесса есть статус: NEED_WORK(нуждается в работе), WORKING(работает), IDLE (не получил работу), QUIT (завершил работу полностью). 
В начале работы программы у главного процесса статус «WORKING», у остальных – «NEED_WORK». Главный процесс распределяет работу между всеми, 
статус всех процессов меняется на «WORKING». 
Если происходит подъем наверх и выясняется, что ветвь пройдена полностью, процесс меняет статус на «NEED_WORK».
На каждом узле ветвлений происходит обмен сообщениями (каждый процесс отправляет в нескольких сообщениях результаты своей работы 
каждому процессу). 
Работа заканчивается, когда обработаны (просчитаны и отброшены) все возможные ветви кроме одной – самого короткого маршрута.

 Функция do_work(struct tsp_state *state) включает в себя работу одного процесса по подсчету длины нового пути. 
Каждый из процессов считает общую длину своей ветви. 
При этом, если на каком-то этапе длина незаконченной ветви окажется больше «state->ub» - длины самого короткого полного маршрута, 
то происходит шаг наверх. 
Если же ветвь будет  пройдена полностью и маршрут замкнется, то считается его полная длина, если она меньше «state->ub», 
то в «state->ub» записывается эта длина и рассылается всем процессам (функция «send_ub_message(state)»), 
а путь признается самым коротким и записывается в переменную «state->best_tour». Изначально «state->ub» = INT_MAX. 
Стоимость текущего пути хранится в переменной «state->cost».

Функция service_work_request(struct tsp_state *state, MPI_Status status) включает в себя распределение работы между процессами. 
Происходит проверка на наличие необработанных ветвей, если такие есть, то новый путь отправляется процессу на подсчет его длины.

Функция service_pending_messages (struct tsp_state *state) включает в себя реализацию приема и обработки сообщений, 
отправленных  процессами во время работы. Типы сообщений отличаются тэгами:
сообщение с новым самым коротким маршрутом  - UB_TAG,сообщение с его длиной - BEST_PATH_REQ_TAG, 
сообщение  с запросом на обработку нового пути - WORK_REQ_TAG. 
В зависимости от сообщения вызывается соответствующий метод, или меняется глобальная переменная. 

Функция request_work (struct tsp_state *state) реализует отправку процессом сообщения с тэгом WORK_REQ_TAG в случае,
когда его статус был NEED_WORK. Далее это сообщение обрабатывается функцией service_pending_messages, 
которая распознает тип сообщения и вызывает функцию service_work_request, которая находит и отправляет путь, 
длину которого надо вычислить текущему процессу. Он, в свою очередь, в данной функции принимает это сообщение, 
меняет свой статус на WORKING, обнуляет длину текущего пути, а сам путь, который надо посчитать записывает в переменную &state->tour.

Функция send_ub_message (struct tsp_state *state) отправляет всем процессам новую минимальную длину пути. 




