# AdvEmbSoft : Bike-computer 
## Student
- Amez-Droz Jonathan
- Loup Olivia
## Info
- school : MSE HES-SO
- Year : 1

## Short description of the project
Project to develop a bike computer in C++.
Group working to learn how to improve a quality program depend of :
- Tested
- CI/CD
- Quality

## Part 1
### Question 1
When you run multiple tests for computing the response time of the reset event, you may observe the following:

1. There is a large variation in the response time values, from a few milliseconds to about 
2. If you do not press long enough on the push button, the event may be missed and no reset happens.

Based on the program itself and on the task scheduling, explain these two behaviors. Explain also why such behaviors may be problematic.

1) from a few miliseconds to about 799ms. It's because the static task scheduling works with a fixed period for each task.
Since the reset task has a period of 800ms, if you happend to hit the switch exactly 1ms after the end of the reset task, you have
to wait for 799ms until the input can be red again.

2) It's also because of the task scheduler. If you happen to click in between two reset tasks, the input won't be red and nothing will happen.

![Screenshot from 2024-10-22 01-32-50](https://github.com/user-attachments/assets/7969784a-385c-4cdf-b618-e00cea3fb4b3)

# Part 2
Test for part 2 mbed test -m DISCO_H747I -t GCC_ARM -n tests-bike-computer-bike-system --compile --run -v

### Question  1 
If you print CPU statistics at the end of every major cycle (in the super-loop), what CPU usage do you observe? How can you explain the observed CPU uptime? : 
![image](https://github.com/user-attachments/assets/e9345101-e321-40f7-8071-b02acbc93108)

The CPU usage is very close to 100%. This is because we are doing an "active wait". By using the while loop as a scheduling method, we keep the system active all the time.


### Question 2
If you run the program after the change from busy wait to sleep calls, what CPU usage do you observe? How can you explain the observed CPU uptime?

![image](https://github.com/user-attachments/assets/b46908c0-bac0-4191-ab7d-eed3edb2646a)

By modifying the active wait with sleep_for() we can reduce the CPU usage time to 75%. This is because the CPU gets into sleep mode before executing the next instruction. The CPU usage is better but it must be optimized by adding a scheduler.


### Question 3
If you run the static_scheduling_with_event program, what CPU usage do you observe? How can you explain the observed CPU uptime?

![image](https://github.com/user-attachments/assets/b2fa0c13-f950-402a-8bc7-b565dbb1b8f3)

The CPU usage time drop as low as 1%. This is achieved thank to the implementation of interrupts


### Question 4
When you run multiple tests for computing the response time of the reset event, what do you observe? Is there an improvement as compared to the static_scheduling::BikeSystem implementation?

If you do not press long enough on the push button, the event may be missed and no reset happens.
Based on the program itself and on the task scheduling, explain these two behaviors. Explain also why such behaviors may be problematic.

With event driven implementation, it takes only 1-2us to perform the reset. But the response time can be a bit longer.

![image](https://github.com/user-attachments/assets/1423de78-712a-4959-87f5-f744744e043d)

Without events, the reset tasks takes 1us to respond. But the downside is that you have to press the button during the polling window which appens every 200ms.

![image](https://github.com/user-attachments/assets/ea48e775-f115-4762-aa06-b2187c68d888)

For a safety feature like a reset button, it's very important to have it even-drivent. This ensures that the reset can happen anytime.
