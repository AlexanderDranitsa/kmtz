
1. !!! Please, get to know about SWEEP and it's commands (google.com) !!!
2. You're welcome to improve these src codes. This program is written in a very frivolous style. I'll leave it as is :)
3. How to use: there are two ways: you inline args or you do not
   args = commands as they're written in datasheet
   3.1 You inline args
       if you pass parameters when runnig this program, it will send it to `parse_and_send_command`
       there these parameters will be parsed in natural order and sent to SWEEP
       this mode was added to be able do this: `./%THIS_APP% DS_STEP_POINTS`
           so that you can skip any i/o while programm runs and just set all args at the start
   3.2 You do not inline args
       you start this app
       you read output and stick to do what it asks for
       "enter a command" -> type command as it's written in datasheet write to console
       then this command will be send to SWEEP and it will execute it
       then you can type anoter command
       type `EXIT` to exit
4. first example
   - `./app`
   - % wait %
   - % when you see it asks you to type a command %:
       - `MS 05`      -> set speed = 5
        - `ID`         -> watch ID
        - `LR 02`      -> set laser rate = 2
        - `DS 10 1000` -> start scan with rotate step 10 degrees and 1000 points per plane
        - `EXIT`       -> exit

5. second example
    - `./app DS_10_1000`
    SWEEP will do same scan (see prev example) using it's setings from previous run
    

![example](https://github.com/uwndrfl/kmtz/blob/master/src/example.gif)
