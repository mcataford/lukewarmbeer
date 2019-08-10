from invoke import task

class NoDeviceFoundException(Exception):
    pass

def _find_board(ctx):
    board_list = ctx.run('bin/arduino-cli board list', hide='stdout')
    lines = board_list.stdout.split('\n')

    for line in lines[1:]:
        cleaned_line = [token for token in line.replace('Serial Port', '').strip().split(' ') if token]
        
        if not cleaned_line:
            continue

        port, device = cleaned_line
        
        # This assumes that only one device is used at a time. >:(
        if device != 'Unknown':
            return port

    raise NoDeviceFoundException()

@task
def upload(ctx):
    try:
        device = _find_board(ctx)
        print(device)
    except NoDeviceFoundException:
        print('No device found')

@task
def compile(ctx):
    ctx.run('echo "holy moly"')
