from receiver import trigger

name=''
def receive_name(n):
    global name
    if n:
        name=n
def check(n):
    global name
    if(name==n):
        trigger('0')
    else:
        trigger('1')
