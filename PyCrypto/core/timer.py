import time


class Timer:
    """Секундомер для замера времени операций."""

    def __init__(self) -> None:
        self._start = time.perf_counter()

    def start(self) -> None:
        self._start = time.perf_counter()

    def elapsed_ms(self) -> float:
        return (time.perf_counter() - self._start) * 1000.0
