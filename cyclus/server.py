"""An asyncronous cyclus server."""
from argparse import ArgumentParser

import cyclus.events
from cyclus.system import asyncio, concurrent_futures, websockets, QUEUE
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


async def run_sim(state, loop, executor):
    """Runs a cyclus simulation in an executor, which should be on another
    thread.
    """
    run_task = loop.run_in_executor(executor, state.run)
    state.tasks['run'] = run_task
    await asyncio.wait([run_task])


async def get_send_data():
    """Asynchronously grabs the next data to send from the queue."""
    print("gsd-0")
    f = cyclus.events.FREQUENCY
    print("gsd-1")
    state = cyclus.events.STATE
    print("gsd-2")
    q = state.send_queue
    print("gsd-3")
    print("seend_q size", q.qsize())
    #while q.empty():
    #    await asyncio.sleep(f)
    #print("gsd-5")
    #data = q.get()
    data = await state.send_queue.get()
    #return data
    print("gsd-6")
    print("gsd-7")
    return data


async def websocket_handler(websocket, path):
    """Sends and recieves data via a websocket."""
    print(0)
    while True:
        print(1)
        recv_task = asyncio.ensure_future(websocket.recv())
        send_task = asyncio.ensure_future(get_send_data())
        print(2)
        done, pending = await asyncio.wait([recv_task, send_task],
                                           return_when=asyncio.FIRST_COMPLETED)
        print("done", done)
        print("pending", pending)
        # handle incoming
        print("recv_task", recv_task)
        if recv_task in done:
            message = recv_task.result()
            #await consumer(message)
            print(message)
        else:
            recv_task.cancel()
        # handle sending of data
        print("send_task", send_task)
        if send_task in done:
            data = send_task.result()
            print("ws: sending data", data)
            await websocket.send(data)
        else:
            print("ws: not sending data")
            send_task.cancel()


async def websockets_server(loop, executor):
    """Runs a cyclus simulation in an executor, which should be on another
    thread.
    """
    #serv_task = loop.run_in_executor(executor, websockets.serve,
    #                                 websocket_handler, 'localhost', 4242)
    #await asyncio.wait([serv_task])
    #server = websockets.serve(websocket_handler, 'localhost', 4242)
    #return server


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


def main(args=None):
    """Main cyclus server entry point."""
    p = make_parser()
    ns = p.parse_args(args=args)
    cyclus.events.STATE = state = SimState(input_file=ns.input_file,
                                           output_path=ns.output_path,
                                           memory_backend=True)
    state.load()

    import logging
    logger = logging.getLogger('websockets.server')
    logger.setLevel(logging.ERROR)
    logger.addHandler(logging.StreamHandler())

    cyclus.events.REPEATING_ACTIONS.append([echo, "repeating task"])
    cyclus.events.REPEATING_ACTIONS.append([sleep, 1])
    cyclus.events.REPEATING_ACTIONS.append([send_table, "Transactions"])
    executor = concurrent_futures.ThreadPoolExecutor(max_workers=16)
    loop = asyncio.get_event_loop()
    server = websockets.serve(websocket_handler, 'localhost', 4242)
    try:
        loop.run_until_complete(asyncio.gather(
            asyncio.ensure_future(run_sim(state, loop, executor)),
            asyncio.ensure_future(action_consumer()),
            #asyncio.ensure_future(websockets_server(loop, executor)),
            asyncio.ensure_future(server),
            ))
    finally:
        loop.close()


if __name__ == '__main__':
    main()