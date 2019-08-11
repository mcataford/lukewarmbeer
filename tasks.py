from invoke import task

class NoDeviceFoundException(Exception):
    pass

SKETCH_PATH = './src/LukeWarmBeer'

def _find_board(ctx):
    board_list = ctx.run('bin/arduino-cli board list', hide='stdout')
    lines = board_list.stdout.split('\n')

    for line in lines[1:]:
        cleaned_line = [token for token in line.replace('Serial Port', '').strip().split(' ') if token]
        
        if not cleaned_line:
            continue
        line_items = cleaned_line
        port, device = line_items[0], line_items[-1]
        
        # This assumes that only one device is used at a time. >:(
        if device != 'Unknown':
            return port

    raise NoDeviceFoundException()

@task
def devices(ctx):
    ctx.run('bin/arduino-cli board list')

@task
def upload(ctx):
    try:
        # We probably should not sudo.
        device_port = _find_board(ctx)
        ctx.run('sudo bin/arduino-cli upload -p {port} -b arduino:avr:uno {path}'.format(port=device_port, path=SKETCH_PATH))
    except NoDeviceFoundException:
        print('No device found')
    except Exception as e:
        print(e)

@task
def compile(ctx):
    ctx.run('bin/arduino-cli compile -b arduino:avr:uno {path}'.format(path=SKETCH_PATH))
