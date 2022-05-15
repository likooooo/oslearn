class Mutex:
    locked = ''

    @thread
    def t1(self):
        while True:
            while self.locked == 'L':
                pass
            self.locked = 'L'
            cs = True
            del cs
            self.locked = ''

    @thread
    def t2(self):
        while True:
            while self.locked == 'L':
                pass
            self.locked = 'L'
            cs = True
            del cs
            self.locked = ''

    @marker
    def mark_t1(self, state):
        if localvar(state, 't1', 'cs'): return 'blue'

    @marker
    def mark_t2(self, state):
        if localvar(state, 't2', 'cs'): return 'green'

    @marker
    def mark_both(self, state):
        if localvar(state, 't1', 'cs') and localvar(state, 't2', 'cs'):
            return 'red'
