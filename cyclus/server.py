"""An asyncronous cyclus server."""
from cyclus.system import curio, QUEUE
from cyclus.events import action_consumer, echo


async def mainbody():
    """Main cyclus server entry point."""
    await QUEUE.put(echo("yo"))
    cons_task = await curio.spawn(action_consumer())
    await cons_task.cancel()


def main():
    """Main cyclus server entry point."""
    curio.run(mainbody(), with_monitor=True)

if __name__ == '__main__':
    main()