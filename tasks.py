from invoke import task

@task
def compile(ctx):
    ctx.run('echo "holy moly"')
