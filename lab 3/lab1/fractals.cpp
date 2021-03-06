#include "fractals.h"

std::condition_variable render_lock;
std::mutex mtx;

static struct pixel {
    int x;
    int y;
    COLORREF color;
};

#define q_reset(q) (q->tail_index.store(0,mem_order),q->front_index.store(0,mem_order))
#define q_front(q) (q->array[q->front_index].load(mem_order))
#define q_empty(q) (q->tail_index.load(mem_order) ? false : true)

#define mem_order std::memory_order_seq_cst
class LineQueue {
public:
    std::mutex mtx;
    std::atomic<pixel*>* array;
    size_t size;
    std::atomic<size_t> front_index;
    std::atomic<size_t> tail_index;
public:
    LineQueue(std::atomic<pixel*>* _array, size_t _size) {
        array = _array;
        size = _size;
        front_index = tail_index = 0;
    }
    void push(pixel* value) {
        mtx.lock();
        int index = tail_index.load(mem_order);
        if (index + 1 <= size) {
            array[index].store(value, mem_order);
            tail_index.fetch_add(1, mem_order);
        }
        else {
            abort();
        }
        mtx.unlock();
    }
    void pop() {
        mtx.lock();
        if (front_index.load(mem_order) + 1 < tail_index.load(mem_order)) {
            front_index.fetch_add(1, mem_order);
        }
        else {
            q_reset(this);
        }
        mtx.unlock();
    }
};

static void process(
    HANDLE pool,
    LineQueue* lineq,
    int src_width_start,
    int src_width,
    int src_height,
    int iterations,
    double step_x,
    double step_y,
    std::complex<double> start,
    std::complex<double> finish
) {

    BYTE color_key = 200;
    COLORREF pixel_color;
    pixel* line;
    for (int i = src_width_start; i < src_width; i++) {
        line = (pixel*)HeapAlloc(pool, HEAP_GENERATE_EXCEPTIONS, src_height * sizeof(pixel));

        for (int j = 0; j < src_height; j++) {
            std::complex<double> Z(0, 0);
            std::complex<double> C(start.real() + step_x * i, start.imag() + step_y * j);
            for (size_t k = 1; k < iterations + 1; k++) {
                Z = Z * Z + C;
                if (std::abs(Z) > 2) {
                    pixel_color = RGB(255 / k, color_key - 128 / k, color_key - 64 / k);
                    break;
                }
                else {
                    pixel_color = RGB(0, 0, 0);
                }
            }
            line[j] = { i,j,pixel_color };
        }
        if (q_empty(lineq)) {
            lineq->push(line);
            render_lock.notify_one();
        }
        else {
            lineq->push(line);
        }
    }
}

static void render(HDC hdc, HANDLE pool, LineQueue* lineq, int lines_count, int pixels_count) {
    std::unique_lock<std::mutex> lock(mtx);
    int line_rendered = 0;
    while (line_rendered < lines_count) {
        render_lock.wait(lock);
        while (!q_empty(lineq)) {
            pixel* line = q_front(lineq);
            for (size_t i = 0; i < pixels_count; i++) {
                SetPixel(hdc, line[i].x, line[i].y, line[i].color);
            }
            line_rendered++;
            HeapFree(pool, 0, line);
            lineq->pop();
        }
    }
    HeapDestroy(pool);
}

void draw_fractal(HDC hdc,
    int src_width,
    int src_height,
    int iterations,
    double zoom
) {
#define init_process_thread(num,start_x,finish_x)\
    std::thread tProcess##num(\
        process,\
        pixel_pool, &q,\
        start_x, finish_x, src_height,\
        iterations,\
        step_x, step_y,\
        start, finish)


    std::complex<double> start(-(2) / zoom, -(1) / zoom);
    std::complex<double> finish((1) / zoom, (1) / zoom);

    double step_x = (finish.real() - start.real()) / src_width;
    double step_y = (finish.imag() - start.imag()) / src_height;

    int src_width_t2_start = src_width / 4;
    int src_width_t3_start = src_width_t2_start * 2;
    int src_width_t4_start = src_width_t2_start * 3;

    size_t max_pool_size = src_width * src_height * sizeof(pixel);
    size_t queue_buffer_size = src_width * sizeof(std::atomic<pixel*>);

    HANDLE pixel_pool = HeapCreate(
        HEAP_GENERATE_EXCEPTIONS,
        src_height * sizeof(pixel),
        0);

    HANDLE queue_pool = HeapCreate(
        HEAP_GENERATE_EXCEPTIONS,
        src_height * sizeof(pixel),
        0);


    LineQueue q((std::atomic<pixel*>*)HeapAlloc(queue_pool, HEAP_GENERATE_EXCEPTIONS, queue_buffer_size), src_width);

    init_process_thread(1, 0, src_width_t2_start);
    init_process_thread(2, src_width_t2_start, src_width_t3_start);
    init_process_thread(3, src_width_t3_start, src_width_t4_start);
    init_process_thread(4, src_width_t4_start, src_width);

    std::thread tRender(render, hdc, pixel_pool, &q, src_width, src_height);

    tProcess1.join();
    tProcess2.join();
    tProcess3.join();
    tProcess4.join();

    tRender.join();


    HeapDestroy(queue_pool);

}