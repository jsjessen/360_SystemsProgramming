#--------------------- s.s file --------------------------

# James Jessen
# 10918967
#
# CptS 360
# Lab #2

.global main, mymain, myprintf

.text
    main:    

# Establish stack frame
        pushl  %ebp
        movl   %esp, %ebp

# (1). Write ASSEMBLY code to call myprintf(FMT)
#      HELP: How does mysum() call printf() in the class notes.
        pushl $FMT
        call myprintf

# (2). Write ASSEMBLY code to call mymain(argc, argv, env)
#      HELP: When crt0.o calls main(int argc, char *argv[], char *env[]), 
#            it passes argc, argv, env to main(). 
#
#       16     12      8      4      0 
#----------------------------------------------------
#     | env | argv | argc | retPC | ebp |
#------------------------------------|---------------
#                                   ebp

        pushl 16(%ebp)  # env
        pushl 12(%ebp)  # argv
        pushl  8(%ebp)  # argc
        call mymain

# (3). Write code to call myprintf(fmt,a,b) 
#      HELP: same as in (1) above
        pushl b
        pushl a
        pushl $fmt
        call myprintf

# (4). Return to caller
        movl  %ebp, %esp
        popl %ebp
        ret

#---------- DATA section of assembly code ---------------
.data
    FMT:    .asciz "main() in assembly call mymain() in C\n"
    a:      .long 1234
    b:      .long 5678
    fmt:    .asciz "a=%d b=%d\n"
#---------  end of s.s file ----------------------------