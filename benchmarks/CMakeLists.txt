set(BENCHMARKS
    TransposeBenchmark
)

foreach(benchmark ${BENCHMARKS})
        add_executable(${benchmark} ${benchmark}.cpp)

        target_link_libraries(${benchmark} computecl)
endforeach(benchmark)
