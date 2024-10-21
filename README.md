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
**When you run multiple tests for computing the response time of the reset event, you may observe the following:

1. There is a large variation in the response time values, from a few milliseconds to about 
2. If you do not press long enough on the push button, the event may be missed and no reset happens.

Based on the program itself and on the task scheduling, explain these two behaviors. Explain also why such behaviors may be problematic.**

1) from a few miliseconds to about 799ms. It's because the static task scheduling works with a fixed period for each task.
Since the reset task has a period of 800ms, if you happend to hit the switch exactly 1ms after the end of the reset task, you have
to wait for 799ms until the input can be red again.

2) It's also because of the task scheduler. If you happen to click in between two reset tasks, the input won't be red and nothing will happen.

![Screenshot from 2024-10-22 01-32-50](https://github.com/user-attachments/assets/7969784a-385c-4cdf-b618-e00cea3fb4b3)


