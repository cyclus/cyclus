"""An asyncronous cyclus server."""
from argparse import ArgumentParser

import cyclus.events
from cyclus.system import asyncio, concurrent_futures, QUEUE
from cyclus.simstate import SimState
from cyclus.events import (action_consumer, echo, register_tables, send_table,
    sleep)


def make_parser():
    """Makes the argument parser for the cyclus server."""
    p = ArgumentParser("cyclus", description="Cyclus Server CLI")
    p.add_argument('-o', '--output-path', dest='output_path',
                   default=None, help='output path')
    p.add_argument('input_file', help='path to input file')
    return p


async def cyclus_client(client, addr):
    state = cyclus.events.STATE
    q = state.send_queue
    print('Connection from', addr)
    while True:
        print("send queue size", q.qsize())
        while not q.empty():
            data = q.get()
            await client.sendall(data.encode())
            #await q.task_done()
            print("send queue size", q.qsize())
        data = await client.recv(100)
        print(data)
        #if not data:
        #    break
    print('Connection closed')


async def run_sim(state):
    await asyncio.run_in_thread(state.run)


async def mainbody(state=None):
    """Main cyclus server entry point."""
    if state is None:
        state = cyclus.events.STATE
    #await QUEUE.put(register_tables("Transactions"))
    #QUEUE.put(register_tables("Transactions"))
    cyclus.events.REPEATING_ACTIONS.append([echo, "repeating task"])
    cyclus.events.REPEATING_ACTIONS.append([sleep, 1])
    cyclus.events.REPEATING_ACTIONS.append([send_table, "Transactions"])
    cons_task = await curio.spawn(action_consumer())
    #cons_task = await curio.run_in_thread(action_consumer)
    #run_task = await curio.run_in_thread(run_sim, state)
    tcp_task = await curio.spawn(curio.tcp_server('localhost', 4242,
                                                  cyclus_client), daemon=True)
    print("started tcp")
    #run_task = await curio.run_in_thread(state.run)
    run_task = await curio.spawn(run_sim(state))
    state.tasks['run'] = run_task
    print("started sim")
    #cons_task.join()
    #run_task.join()
    #await QUEUE.put(echo("yo"))
    #await cons_task.cancel()



async def mb2(client, addr):
    state = cyclus.events.STATE

    QUEUE.put(register_tables("Transactions"))
    cyclus.events.REPEATING_ACTIONS.append([echo, "repeating task"])
    cyclus.events.REPEATING_ACTIONS.append([sleep, 1])
    cyclus.events.REPEATING_ACTIONS.append([send_table, "Transactions"])
    cons_task = await curio.spawn(action_consumer())

    run_task = await curio.spawn(run_sim(state))
    state.tasks['run'] = run_task
    print("started sim")

    # tcp part
    q = state.send_queue
    print('Connection from', addr)
    while True:
        while not q.empty():
            data = await q.get()
            await client.sendall(data)
            await q.task_done()
        #data = await client.recv(1000)
        #if not data:
        #    break
    print('Connection closed')


def main(args=None):
    """Main cyclus server entry point."""
    p = make_parser()
    ns = p.parse_args(args=args)
    cyclus.events.STATE = state = SimState(input_file=ns.input_file,
                                           output_path=ns.output_path,
                                           memory_backend=True)
    state.load()

    executor = concurrent_futures.ThreadPoolExecutor(max_workers=5)
    loop = asyncio.get_event_loop()
    try:
        loop.run_until_complete(asyncio.gather(
            o,
            ))
    finally:
        loop.close()


if __name__ == '__main__':
    main()