       0:  movl    $0x2a, %eax
       5:  testq   %rdi, %rdi
       8:  je      0x1a <_Z3fooPiii+0x1a>
       a:  testl   %edx, %edx
       c:  jle     0x1b <_Z3fooPiii+0x1b>
       e:  movl    %edx, %ecx
      10:  xorl    $0x7fffffff, %ecx       # imm = 0x7FFFFFFF
      16:  cmpl    %esi, %ecx
      18:  jge     0x1b <_Z3fooPiii+0x1b>
      1a:  retq
      1b:  testl   %edx, %edx
      1d:  jns     0x2a <_Z3fooPiii+0x2a>
      1f:  movl    $0x80000000, %ecx       # imm = 0x80000000
      24:  subl    %edx, %ecx
      26:  cmpl    %esi, %ecx
      28:  jg      0x1a <_Z3fooPiii+0x1a>
      2a:  addl    %esi, %edx
      2c:  js      0x1a <_Z3fooPiii+0x1a>
      2e:  movl    %edx, %eax
      30:  movl    (%rdi,%rax,4), %eax
      33:  retq
