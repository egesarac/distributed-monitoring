import os
import time

from z3 import *
from z3.z3 import ForAll

# set_param('parallel.enable', True)
set_option(rational_to_decimal=True)
set_option(max_args=100000000, max_lines=10000000000, max_depth=100000000, max_visited=10000000)

def prog_always_conjunction(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # pad = min(2 * eps, len(data_0) - 1)
    pad = 2
    peps = pad * eps

    # initialize signal duration and segment duration
    sigDur = len(data_0) - 1
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:
        return

    var_0 = {0 : True}
    for i in range(1, len(data_0)):
        if (data_0[i][1] != data_0[i - 1][1]):
            var_0[int(data_0[i][0])] = True
    var_0[int(data_0[-1][0])] = True

    var_1 = {0 : True}
    for i in range(1, len(data_1)):
        if (data_1[i][1] != data_1[i - 1][1]):
            var_1[int(data_1[i][0])] = True
    var_1[int(data_1[-1][0])] = True

    # encoding
    i = 0
    entryFound = True
    flag = True

    while entryFound:

        # Terminate if segment count is reached
        if i == segCount:
            return flag

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = max(0, int((i * segDur) - eps))
        # segmentUpperBound = int((i + 1) * segDur)
        segmentUpperBound = min(sigDur, int((i + 1) * segDur + eps))

        timestamps_0 = []
        segvar_0 = []
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_0) - 1:

                s.add(sig0(j) == data_0[j][1])
                for k in range(pad):
                    timestamps_0.append(j * pad + k)

                # if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                #     var_0.append(j)

                if (j == segmentLowerBound or (data_0[j][0] in var_0 and data_0[j][0] > segmentLowerBound)):
                    segvar_0.append(int(data_0[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_0) > 0 and segvar_0[-1] < segmentUpperBound):
            segvar_0.append(segmentUpperBound)

        timestamps_1 = []
        segvar_1 = []
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_1) - 1:

                s.add(sig1(j) == data_1[j][1])
                for k in range(pad):
                    timestamps_1.append(j * pad + k)

                # if (j > 0 and data_1[j][1] != data_1[j - 1][1]):
                #     var_1.append(j)

                if (j == segmentLowerBound or (data_1[j][0] in var_1 and data_1[j][0] > segmentLowerBound)):
                    segvar_1.append(int(data_1[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_1) > 0 and segvar_1[-1] < segmentUpperBound):
            segvar_1.append(segmentUpperBound)              

        if not entryFound:

            ##print()
            pass

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings (implicit piece-wise constant interpolation w.r.t. local clock values)
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c0(i) >= 0, c0(i) < data_0[-1][0])
                    # Or([c0(i) == (min(timestamps_0[-1]//pad, (max(0, ((i - peps) + j)//pad)))) for j in range(1, 2 * int(peps))])
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c0(min(timestamps_0[-1], max(0, i * pad - peps + j))) == i for j in range(1, 2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                    # for i in range(int(data_0[-1][0]))
                ]
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c1(i) >= 0, c1(i) < data_1[-1][0])
                    # Or([c1(i) == (min(timestamps_1[-1]//pad, (max(0, ((i - peps) + j)//pad)))) for j in range(1, 2 * int(peps))])
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c1(min(timestamps_1[-1], max(0, i * pad - peps + j))) == i for j in range(2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                    # for i in range(int(data_1[-1][0]))
                ]
            )
        )
        
        # retiming preserves signal variability
        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_0[i] <= c0(j), c0(j) < segvar_0[i + 1])
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_0) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_1[i] <= c1(j), c1(j) < segvar_1[i + 1])
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_1) - 1)
                ]
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # non-concurrent edges stay non-concurrent ???
        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c0(i) == segvar_0[j], c0(i) != c0(i - 1)) for j in range(1, len(segvar_0) - 1)]),
                            Or([And(c1(i) == segvar_1[j], c1(i) != c1(i - 1)) for j in range(1, len(segvar_1) - 1)])
                        ),
                        And(c0(i) - c1(i) < eps, c0(i) - c1(i) > -eps)
                    )
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps_0[0], v <= timestamps_0[-1]))

        s.add(
            Implies(
                And(v >= timestamps_0[0], v <= timestamps_0[-1]),
                z3Interpolate(c_flow, v) < 2
            )
        )

        # print(s.assertions())
        if s.check() == sat:
            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            # print(m)
            ##print("unsat in segment", i)
            flag = False
            # terminate after unsat
            return flag

        elif i <= segCount:
            ##print("sat in segment", i)
            flag = True

        s.reset()

    return flag



def prog_always_disjunction4(eps, segCount, data_0, data_1, data_2, data_3):

    # pad = min(2 * eps, len(data_0) - 1)
    pad = 2
    peps = pad * eps

    # initialize signal duration and segment duration
    sigDur = len(data_0) - 1
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:
        return

    var_0 = {0 : True}
    for i in range(1, len(data_0)):
        if (data_0[i][1] != data_0[i - 1][1]):
            var_0[int(data_0[i][0])] = True
    var_0[int(data_0[-1][0])] = True

    var_1 = {0 : True}
    for i in range(1, len(data_1)):
        if (data_1[i][1] != data_1[i - 1][1]):
            var_1[int(data_1[i][0])] = True
    var_1[int(data_1[-1][0])] = True

    var_2 = {0 : True}
    for i in range(1, len(data_2)):
        if (data_2[i][1] != data_2[i - 1][1]):
            var_2[int(data_2[i][0])] = True
    var_2[int(data_2[-1][0])] = True

    var_3 = {0 : True}
    for i in range(1, len(data_3)):
        if (data_3[i][1] != data_3[i - 1][1]):
            var_2[int(data_3[i][0])] = True
    var_3[int(data_3[-1][0])] = True

    # encoding
    i = 0
    entryFound = True
    flag = True

    while entryFound:

        # Terminate if segment count is reached
        if i == segCount:
            return flag

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = max(0, int((i * segDur) - eps))
        # segmentUpperBound = int((i + 1) * segDur)
        segmentUpperBound = min(sigDur, int((i + 1) * segDur + eps))

        timestamps_0 = []
        segvar_0 = []
        sig0 = Function("sig0", IntSort(), RealSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_0) - 1:

                s.add(sig0(j) == data_0[j][1])
                for k in range(pad):
                    timestamps_0.append(j * pad + k)

                # if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                #     var_0.append(j)

                if (j == segmentLowerBound or (data_0[j][0] in var_0 and data_0[j][0] > segmentLowerBound)):
                    segvar_0.append(int(data_0[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_0) > 0 and segvar_0[-1] < segmentUpperBound):
            segvar_0.append(segmentUpperBound)

        timestamps_1 = []
        segvar_1 = []
        sig1 = Function("sig1", IntSort(), RealSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_1) - 1:

                s.add(sig1(j) == data_1[j][1])
                for k in range(pad):
                    timestamps_1.append(j * pad + k)

                # if (j > 0 and data_1[j][1] != data_1[j - 1][1]):
                #     var_1.append(j)

                if (j == segmentLowerBound or (data_1[j][0] in var_1 and data_1[j][0] > segmentLowerBound)):
                    segvar_1.append(int(data_1[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_1) > 0 and segvar_1[-1] < segmentUpperBound):
            segvar_1.append(segmentUpperBound)              


        timestamps_2 = []
        segvar_2 = []
        sig2 = Function("sig2", IntSort(), RealSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_2) - 1:

                s.add(sig2(j) == data_2[j][1])
                for k in range(pad):
                    timestamps_2.append(j * pad + k)

                # if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                #     var_0.append(j)

                if (j == segmentLowerBound or (data_2[j][0] in var_2 and data_2[j][0] > segmentLowerBound)):
                    segvar_2.append(int(data_2[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_2) > 0 and segvar_2[-1] < segmentUpperBound):
            segvar_2.append(segmentUpperBound)


        timestamps_3 = []
        segvar_3 = []
        sig3 = Function("sig3", IntSort(), RealSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_3) - 1:

                s.add(sig3(j) == data_3[j][1])
                for k in range(pad):
                    timestamps_3.append(j * pad + k)

                # if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                #     var_0.append(j)

                if (j == segmentLowerBound or (data_3[j][0] in var_3 and data_3[j][0] > segmentLowerBound)):
                    segvar_3.append(int(data_3[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_3) > 0 and segvar_3[-1] < segmentUpperBound):
            segvar_3.append(segmentUpperBound)

        if not entryFound:

            ##print()
            pass

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings (implicit piece-wise constant interpolation w.r.t. local clock values)
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c0(i) >= 0, c0(i) < data_0[-1][0])
                    # Or([c0(i) == (min(timestamps_0[-1]//pad, (max(0, ((i - peps) + j)//pad)))) for j in range(1, 2 * int(peps))])
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c0(min(timestamps_0[-1], max(0, i * pad - peps + j))) == i for j in range(1, 2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                    # for i in range(int(data_0[-1][0]))
                ]
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c1(i) >= 0, c1(i) < data_1[-1][0])
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c1(min(timestamps_1[-1], max(0, i * pad - peps + j))) == i for j in range(2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                ]
            )
        )

        c2 = Function("c2", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c2(i) >= 0, c2(i) < data_2[-1][0])
                    for i in range(timestamps_2[0], timestamps_2[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c2(min(timestamps_2[-1], max(0, i * pad - peps + j))) == i for j in range(2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                ]
            )
        )

        c3 = Function("c3", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c3(i) >= 0, c3(i) < data_3[-1][0])
                    for i in range(timestamps_3[0], timestamps_3[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c3(min(timestamps_3[-1], max(0, i * pad - peps + j))) == i for j in range(2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                ]
            )
        )
        
        # retiming preserves signal variability
        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_0[i] <= c0(j), c0(j) < segvar_0[i + 1])
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_0) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_1[i] <= c1(j), c1(j) < segvar_1[i + 1])
                            for j in range(timestamps_1[0], timestamps_1[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_1) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_2[i] <= c2(j), c2(j) < segvar_2[i + 1])
                            for j in range(timestamps_2[0], timestamps_2[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_2) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_3[i] <= c3(j), c3(j) < segvar_3[i + 1])
                            for j in range(timestamps_3[0], timestamps_3[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_3) - 1)
                ]
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )
        
        s.add(
            And(
                [
                    And(c0(i) - c2(i) <= eps, c0(i) - c2(i) >= -eps)
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    And(c1(i) - c2(i) <= eps, c1(i) - c2(i) >= -eps)
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        s.add(And([And(c0(i) - c3(i) <= eps, c0(i) - c3(i) >= -eps) for i in range(timestamps_3[0], timestamps_3[-1] + 1)]))
        s.add(And([And(c1(i) - c3(i) <= eps, c1(i) - c3(i) >= -eps) for i in range(timestamps_3[0], timestamps_3[-1] + 1)]))
        s.add(And([And(c2(i) - c3(i) <= eps, c2(i) - c3(i) >= -eps) for i in range(timestamps_3[0], timestamps_3[-1] + 1)]))

        # non-concurrent edges stay non-concurrent ???
        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c0(i) == segvar_0[j], c0(i) != c0(i - 1)) for j in range(1, len(segvar_0) - 1)]),
                            Or([And(c1(i) == segvar_1[j], c1(i) != c1(i - 1)) for j in range(1, len(segvar_1) - 1)])
                        ),
                        And(c0(i) - c1(i) < eps, c0(i) - c1(i) > -eps)
                    )
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c0(i) == segvar_0[j], c0(i) != c0(i - 1)) for j in range(1, len(segvar_0) - 1)]),
                            Or([And(c2(i) == segvar_2[j], c2(i) != c2(i - 1)) for j in range(1, len(segvar_2) - 1)])
                        ),
                        And(c0(i) - c2(i) < eps, c0(i) - c2(i) > -eps)
                    )
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c1(i) == segvar_1[j], c1(i) != c1(i - 1)) for j in range(1, len(segvar_1) - 1)]),
                            Or([And(c2(i) == segvar_2[j], c2(i) != c2(i - 1)) for j in range(1, len(segvar_2) - 1)])
                        ),
                        And(c1(i) - c2(i) < eps, c1(i) - c2(i) > -eps)
                    )
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c0(i) == segvar_0[j], c0(i) != c0(i - 1)) for j in range(1, len(segvar_0) - 1)]),
                            Or([And(c3(i) == segvar_3[j], c3(i) != c3(i - 1)) for j in range(1, len(segvar_3) - 1)])
                        ),
                        And(c0(i) - c3(i) < eps, c0(i) - c3(i) > -eps)
                    )
                    for i in range(timestamps_3[0], timestamps_3[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c1(i) == segvar_1[j], c1(i) != c1(i - 1)) for j in range(1, len(segvar_1) - 1)]),
                            Or([And(c3(i) == segvar_3[j], c3(i) != c3(i - 1)) for j in range(1, len(segvar_3) - 1)])
                        ),
                        And(c1(i) - c3(i) < eps, c1(i) - c3(i) > -eps)
                    )
                    for i in range(timestamps_3[0], timestamps_3[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c2(i) == segvar_2[j], c2(i) != c2(i - 1)) for j in range(1, len(segvar_2) - 1)]),
                            Or([And(c3(i) == segvar_3[j], c3(i) != c3(i - 1)) for j in range(1, len(segvar_3) - 1)])
                        ),
                        And(c2(i) - c3(i) < eps, c2(i) - c3(i) > -eps)
                    )
                    for i in range(timestamps_3[0], timestamps_3[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j), c2(i) <= c2(j), c3(i) <= c3(j)))
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), RealSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)) + sig2(c2(i)) + sig3(c3(i)))
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps_0[0], v <= timestamps_0[-1]))

        s.add(
            Implies(
                And(v >= timestamps_0[0], v <= timestamps_0[-1]),
                z3Interpolate(c_flow, v) == 0
            )
        )

        # print(s.assertions())
        if s.check() == sat:
            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            # print(m)
            ##print("unsat in segment", i)
            flag = False
            # terminate after unsat
            return flag

        elif i <= segCount:
            ##print("sat in segment", i)
            flag = True

        s.reset()

    return flag


def prog_eventually_conjunction4(eps, segCount, data_0, data_1, data_2, data_3):

    # pad = min(2 * eps, len(data_0) - 1)
    pad = 2
    peps = pad * eps

    # initialize signal duration and segment duration
    sigDur = len(data_0) - 1
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:
        return

    var_0 = {0 : True}
    for i in range(1, len(data_0)):
        if (data_0[i][1] != data_0[i - 1][1]):
            var_0[int(data_0[i][0])] = True
    var_0[int(data_0[-1][0])] = True

    var_1 = {0 : True}
    for i in range(1, len(data_1)):
        if (data_1[i][1] != data_1[i - 1][1]):
            var_1[int(data_1[i][0])] = True
    var_1[int(data_1[-1][0])] = True

    var_2 = {0 : True}
    for i in range(1, len(data_2)):
        if (data_2[i][1] != data_2[i - 1][1]):
            var_2[int(data_2[i][0])] = True
    var_2[int(data_2[-1][0])] = True

    var_3 = {0 : True}
    for i in range(1, len(data_3)):
        if (data_3[i][1] != data_3[i - 1][1]):
            var_2[int(data_3[i][0])] = True
    var_3[int(data_3[-1][0])] = True

    # encoding
    i = 0
    entryFound = True
    flag = True

    while entryFound:

        # Terminate if segment count is reached
        if i == segCount:
            return flag

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = max(0, int((i * segDur) - eps))
        # segmentUpperBound = int((i + 1) * segDur)
        segmentUpperBound = min(sigDur, int((i + 1) * segDur + eps))

        timestamps_0 = []
        segvar_0 = []
        sig0 = Function("sig0", IntSort(), RealSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_0) - 1:

                s.add(sig0(j) == data_0[j][1])
                for k in range(pad):
                    timestamps_0.append(j * pad + k)

                # if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                #     var_0.append(j)

                if (j == segmentLowerBound or (data_0[j][0] in var_0 and data_0[j][0] > segmentLowerBound)):
                    segvar_0.append(int(data_0[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_0) > 0 and segvar_0[-1] < segmentUpperBound):
            segvar_0.append(segmentUpperBound)

        timestamps_1 = []
        segvar_1 = []
        sig1 = Function("sig1", IntSort(), RealSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_1) - 1:

                s.add(sig1(j) == data_1[j][1])
                for k in range(pad):
                    timestamps_1.append(j * pad + k)

                # if (j > 0 and data_1[j][1] != data_1[j - 1][1]):
                #     var_1.append(j)

                if (j == segmentLowerBound or (data_1[j][0] in var_1 and data_1[j][0] > segmentLowerBound)):
                    segvar_1.append(int(data_1[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_1) > 0 and segvar_1[-1] < segmentUpperBound):
            segvar_1.append(segmentUpperBound)              


        timestamps_2 = []
        segvar_2 = []
        sig2 = Function("sig2", IntSort(), RealSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_2) - 1:

                s.add(sig2(j) == data_2[j][1])
                for k in range(pad):
                    timestamps_2.append(j * pad + k)

                # if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                #     var_0.append(j)

                if (j == segmentLowerBound or (data_2[j][0] in var_2 and data_2[j][0] > segmentLowerBound)):
                    segvar_2.append(int(data_2[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_2) > 0 and segvar_2[-1] < segmentUpperBound):
            segvar_2.append(segmentUpperBound)


        timestamps_3 = []
        segvar_3 = []
        sig3 = Function("sig3", IntSort(), RealSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_3) - 1:

                s.add(sig3(j) == data_3[j][1])
                for k in range(pad):
                    timestamps_3.append(j * pad + k)

                # if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                #     var_0.append(j)

                if (j == segmentLowerBound or (data_3[j][0] in var_3 and data_3[j][0] > segmentLowerBound)):
                    segvar_3.append(int(data_3[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_3) > 0 and segvar_3[-1] < segmentUpperBound):
            segvar_3.append(segmentUpperBound)

        if not entryFound:

            ##print()
            pass

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings (implicit piece-wise constant interpolation w.r.t. local clock values)
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c0(i) >= 0, c0(i) < data_0[-1][0])
                    # Or([c0(i) == (min(timestamps_0[-1]//pad, (max(0, ((i - peps) + j)//pad)))) for j in range(1, 2 * int(peps))])
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c0(min(timestamps_0[-1], max(0, i * pad - peps + j))) == i for j in range(1, 2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                    # for i in range(int(data_0[-1][0]))
                ]
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c1(i) >= 0, c1(i) < data_1[-1][0])
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c1(min(timestamps_1[-1], max(0, i * pad - peps + j))) == i for j in range(2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                ]
            )
        )

        c2 = Function("c2", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c2(i) >= 0, c2(i) < data_2[-1][0])
                    for i in range(timestamps_2[0], timestamps_2[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c2(min(timestamps_2[-1], max(0, i * pad - peps + j))) == i for j in range(2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                ]
            )
        )

        c3 = Function("c3", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c3(i) >= 0, c3(i) < data_3[-1][0])
                    for i in range(timestamps_3[0], timestamps_3[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c3(min(timestamps_3[-1], max(0, i * pad - peps + j))) == i for j in range(2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                ]
            )
        )
        
        # retiming preserves signal variability
        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_0[i] <= c0(j), c0(j) < segvar_0[i + 1])
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_0) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_1[i] <= c1(j), c1(j) < segvar_1[i + 1])
                            for j in range(timestamps_1[0], timestamps_1[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_1) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_2[i] <= c2(j), c2(j) < segvar_2[i + 1])
                            for j in range(timestamps_2[0], timestamps_2[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_2) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_3[i] <= c3(j), c3(j) < segvar_3[i + 1])
                            for j in range(timestamps_3[0], timestamps_3[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_3) - 1)
                ]
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )
        
        s.add(
            And(
                [
                    And(c0(i) - c2(i) <= eps, c0(i) - c2(i) >= -eps)
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    And(c1(i) - c2(i) <= eps, c1(i) - c2(i) >= -eps)
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        s.add(And([And(c0(i) - c3(i) <= eps, c0(i) - c3(i) >= -eps) for i in range(timestamps_3[0], timestamps_3[-1] + 1)]))
        s.add(And([And(c1(i) - c3(i) <= eps, c1(i) - c3(i) >= -eps) for i in range(timestamps_3[0], timestamps_3[-1] + 1)]))
        s.add(And([And(c2(i) - c3(i) <= eps, c2(i) - c3(i) >= -eps) for i in range(timestamps_3[0], timestamps_3[-1] + 1)]))

        # non-concurrent edges stay non-concurrent ???
        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c0(i) == segvar_0[j], c0(i) != c0(i - 1)) for j in range(1, len(segvar_0) - 1)]),
                            Or([And(c1(i) == segvar_1[j], c1(i) != c1(i - 1)) for j in range(1, len(segvar_1) - 1)])
                        ),
                        And(c0(i) - c1(i) < eps, c0(i) - c1(i) > -eps)
                    )
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c0(i) == segvar_0[j], c0(i) != c0(i - 1)) for j in range(1, len(segvar_0) - 1)]),
                            Or([And(c2(i) == segvar_2[j], c2(i) != c2(i - 1)) for j in range(1, len(segvar_2) - 1)])
                        ),
                        And(c0(i) - c2(i) < eps, c0(i) - c2(i) > -eps)
                    )
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c1(i) == segvar_1[j], c1(i) != c1(i - 1)) for j in range(1, len(segvar_1) - 1)]),
                            Or([And(c2(i) == segvar_2[j], c2(i) != c2(i - 1)) for j in range(1, len(segvar_2) - 1)])
                        ),
                        And(c1(i) - c2(i) < eps, c1(i) - c2(i) > -eps)
                    )
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c0(i) == segvar_0[j], c0(i) != c0(i - 1)) for j in range(1, len(segvar_0) - 1)]),
                            Or([And(c3(i) == segvar_3[j], c3(i) != c3(i - 1)) for j in range(1, len(segvar_3) - 1)])
                        ),
                        And(c0(i) - c3(i) < eps, c0(i) - c3(i) > -eps)
                    )
                    for i in range(timestamps_3[0], timestamps_3[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c1(i) == segvar_1[j], c1(i) != c1(i - 1)) for j in range(1, len(segvar_1) - 1)]),
                            Or([And(c3(i) == segvar_3[j], c3(i) != c3(i - 1)) for j in range(1, len(segvar_3) - 1)])
                        ),
                        And(c1(i) - c3(i) < eps, c1(i) - c3(i) > -eps)
                    )
                    for i in range(timestamps_3[0], timestamps_3[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c2(i) == segvar_2[j], c2(i) != c2(i - 1)) for j in range(1, len(segvar_2) - 1)]),
                            Or([And(c3(i) == segvar_3[j], c3(i) != c3(i - 1)) for j in range(1, len(segvar_3) - 1)])
                        ),
                        And(c2(i) - c3(i) < eps, c2(i) - c3(i) > -eps)
                    )
                    for i in range(timestamps_3[0], timestamps_3[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j), c2(i) <= c2(j), c3(i) <= c3(j)))
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), RealSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)) + sig2(c2(i)) + sig3(c3(i)))
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps_0[0], v <= timestamps_0[-1]))

        s.add(
            ForAll(v,
                   Implies(
                        And(v >= timestamps_0[0], v <= timestamps_0[-1]),
                        z3Interpolate(c_flow, v) < 4
                    )
                   )
        )

        # print(s.assertions())
        if s.check() == sat:
            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            # print(m)
            ##print("unsat in segment", i)
            flag = False
            # terminate after unsat
            return flag

        elif i <= segCount:
            ##print("sat in segment", i)
            flag = True

        s.reset()

    return flag



def prog_eventually_conjunction(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # pad = min(2 * eps, len(data_0) - 1)
    pad = 2
    peps = pad * eps

    # initialize signal duration and segment duration
    sigDur = len(data_0) - 1
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:
        return

    var_0 = {0 : True}
    for i in range(1, len(data_0)):
        if (data_0[i][1] != data_0[i - 1][1]):
            var_0[int(data_0[i][0])] = True
    var_0[int(data_0[-1][0])] = True

    var_1 = {0 : True}
    for i in range(1, len(data_1)):
        if (data_1[i][1] != data_1[i - 1][1]):
            var_1[int(data_1[i][0])] = True
    var_1[int(data_1[-1][0])] = True

    # encoding
    i = 0
    entryFound = True
    flag = True

    while entryFound:

        # Terminate if segment count is reached
        if i == segCount:
            return flag

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = max(0, int((i * segDur) - eps))
        # segmentUpperBound = int((i + 1) * segDur)
        segmentUpperBound = min(sigDur, int((i + 1) * segDur + eps))

        timestamps_0 = []
        segvar_0 = []
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_0) - 1:

                s.add(sig0(j) == data_0[j][1])
                for k in range(pad):
                    timestamps_0.append(j * pad + k)

                # if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                #     var_0.append(j)

                if (j == segmentLowerBound or (data_0[j][0] in var_0 and data_0[j][0] > segmentLowerBound)):
                    segvar_0.append(int(data_0[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_0) > 0 and segvar_0[-1] < segmentUpperBound):
            segvar_0.append(segmentUpperBound)

        timestamps_1 = []
        segvar_1 = []
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_1) - 1:

                s.add(sig1(j) == data_1[j][1])
                for k in range(pad):
                    timestamps_1.append(j * pad + k)

                # if (j > 0 and data_1[j][1] != data_1[j - 1][1]):
                #     var_1.append(j)

                if (j == segmentLowerBound or (data_1[j][0] in var_1 and data_1[j][0] > segmentLowerBound)):
                    segvar_1.append(int(data_1[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_1) > 0 and segvar_1[-1] < segmentUpperBound):
            segvar_1.append(segmentUpperBound)              

        if not entryFound:

            ##print()
            pass

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings (implicit piece-wise constant interpolation w.r.t. local clock values)
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c0(i) >= 0, c0(i) < data_0[-1][0])
                    # Or([c0(i) == (min(timestamps_0[-1]//pad, (max(0, ((i - peps) + j)//pad)))) for j in range(1, 2 * int(peps))])
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c0(min(timestamps_0[-1], max(0, i * pad - peps + j))) == i for j in range(1, 2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                    # for i in range(int(data_0[-1][0]))
                ]
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c1(i) >= 0, c1(i) < data_1[-1][0])
                    # Or([c1(i) == (min(timestamps_1[-1]//pad, (max(0, ((i - peps) + j)//pad)))) for j in range(1, 2 * int(peps))])
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c1(min(timestamps_1[-1], max(0, i * pad - peps + j))) == i for j in range(2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                    # for i in range(int(data_1[-1][0]))
                ]
            )
        )
        
        # retiming preserves signal variability
        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_0[i] <= c0(j), c0(j) < segvar_0[i + 1])
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_0) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_1[i] <= c1(j), c1(j) < segvar_1[i + 1])
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_1) - 1)
                ]
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # non-concurrent edges stay non-concurrent ???
        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c0(i) == segvar_0[j], c0(i) != c0(i - 1)) for j in range(1, len(segvar_0) - 1)]),
                            Or([And(c1(i) == segvar_1[j], c1(i) != c1(i - 1)) for j in range(1, len(segvar_1) - 1)])
                        ),
                        And(c0(i) - c1(i) < eps, c0(i) - c1(i) > -eps)
                    )
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps_0[0], v <= timestamps_0[-1]))

        s.add(
            ForAll(v,
                Implies(
                    And(v >= timestamps_0[0], v <= timestamps_0[-1]),
                    z3Interpolate(c_flow, v) < 2
                )
            )
        )

        # print(s.assertions())
        if s.check() == sat:
            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            # print(m)
            ##print("unsat in segment", i)
            flag = False

        elif i <= segCount:
            ##print("sat in segment", i)
            flag = True
            # terminate after sat
            return flag

        s.reset()
        
    return flag


def prog_eventually_disjunction(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # pad = min(2 * eps, len(data_0) - 1)
    pad = 2
    peps = pad * eps

    # initialize signal duration and segment duration
    sigDur = len(data_0) - 1
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:
        return

    var_0 = {0 : True}
    for i in range(1, len(data_0)):
        if (data_0[i][1] != data_0[i - 1][1]):
            var_0[int(data_0[i][0])] = True
    var_0[int(data_0[-1][0])] = True

    var_1 = {0 : True}
    for i in range(1, len(data_1)):
        if (data_1[i][1] != data_1[i - 1][1]):
            var_1[int(data_1[i][0])] = True
    var_1[int(data_1[-1][0])] = True

    # encoding
    i = 0
    entryFound = True
    flag = True

    while entryFound:

        # Terminate if segment count is reached
        if i == segCount:
            return flag

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = max(0, int((i * segDur) - eps))
        # segmentUpperBound = int((i + 1) * segDur)
        segmentUpperBound = min(sigDur, int((i + 1) * segDur + eps))

        timestamps_0 = []
        segvar_0 = []
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_0) - 1:

                s.add(sig0(j) == data_0[j][1])
                for k in range(pad):
                    timestamps_0.append(j * pad + k)

                # if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                #     var_0.append(j)

                if (j == segmentLowerBound or (data_0[j][0] in var_0 and data_0[j][0] > segmentLowerBound)):
                    segvar_0.append(int(data_0[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_0) > 0 and segvar_0[-1] < segmentUpperBound):
            segvar_0.append(segmentUpperBound)

        timestamps_1 = []
        segvar_1 = []
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range(segmentLowerBound, segmentUpperBound + 1):

            if j >= 0 and j < len(data_1) - 1:

                s.add(sig1(j) == data_1[j][1])
                for k in range(pad):
                    timestamps_1.append(j * pad + k)

                # if (j > 0 and data_1[j][1] != data_1[j - 1][1]):
                #     var_1.append(j)

                if (j == segmentLowerBound or (data_1[j][0] in var_1 and data_1[j][0] > segmentLowerBound)):
                    segvar_1.append(int(data_1[j][0]))

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (len(segvar_1) > 0 and segvar_1[-1] < segmentUpperBound):
            segvar_1.append(segmentUpperBound)              

        if not entryFound:

            ##print()
            pass

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings (implicit piece-wise constant interpolation w.r.t. local clock values)
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c0(i) >= 0, c0(i) < data_0[-1][0])
                    # Or([c0(i) == (min(timestamps_0[-1]//pad, (max(0, ((i - peps) + j)//pad)))) for j in range(1, 2 * int(peps))])
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c0(min(timestamps_0[-1], max(0, i * pad - peps + j))) == i for j in range(1, 2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                    # for i in range(int(data_0[-1][0]))
                ]
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    And(c1(i) >= 0, c1(i) < data_1[-1][0])
                    # Or([c1(i) == (min(timestamps_1[-1]//pad, (max(0, ((i - peps) + j)//pad)))) for j in range(1, 2 * int(peps))])
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or([c1(min(timestamps_1[-1], max(0, i * pad - peps + j))) == i for j in range(2 * int(peps) + pad)])
                    for i in range(segmentLowerBound, segmentUpperBound)
                    # for i in range(int(data_1[-1][0]))
                ]
            )
        )
        
        # retiming preserves signal variability
        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_0[i] <= c0(j), c0(j) < segvar_0[i + 1])
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_0) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(segvar_1[i] <= c1(j), c1(j) < segvar_1[i + 1])
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(len(segvar_1) - 1)
                ]
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # non-concurrent edges stay non-concurrent ???
        s.add(
            And(
                [
                    Implies(
                        And(
                            Or([And(c0(i) == segvar_0[j], c0(i) != c0(i - 1)) for j in range(1, len(segvar_0) - 1)]),
                            Or([And(c1(i) == segvar_1[j], c1(i) != c1(i - 1)) for j in range(1, len(segvar_1) - 1)])
                        ),
                        And(c0(i) - c1(i) < eps, c0(i) - c1(i) > -eps)
                    )
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps_0[0], v <= timestamps_0[-1]))

        s.add(
            ForAll(v,
                Implies(
                    And(v >= timestamps_0[0], v <= timestamps_0[-1]),
                    z3Interpolate(c_flow, v) == 0
                )
            )
        )

        # print(s.assertions())
        if s.check() == sat:
            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            # print(m)
            ##print("unsat in segment", i)
            flag = False

        elif i <= segCount:
            ##print("sat in segment", i)
            flag = True
            # terminate after sat
            return flag

        s.reset()

    return flag


def prog_until(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # initialize signal duration and segment duration
    sigDur = len(data_0)
    segDur = sigDur / segCount

    # check for edge cases
    t0 = data_0[0][0]
    t1 = data_0[1][0]

    if sigDur / segCount < t1:

        segCount = sigDur / t1

    if t0 != 0:

        return

    # encoding
    i = 0
    entryFound = True

    while entryFound:

        # flag to be set True if at least one entry is found in the current iteration
        entryFound = False

        # initialize solver
        s = Solver()

        # calculate upper and lower time bound for current segment
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps_0 = []

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0):

                timestamps_0.append(j)

                s.add(sig0(j) == data_0[j][1])

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        timestamps_1 = []

        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1):

                timestamps_1.append(j)

                s.add(sig1(j) == data_1[j][1])

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if not entryFound:

            print()

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c0(i) == ((i - eps) + j) for j in range(2 * int(eps) + 1)])
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c0(timestamps_0[0]) == timestamps_0[0] - 1,
                    c0(timestamps_0[-1]) == timestamps_0[-1] + 1,
                )
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c1(i) == ((i - eps) + j) for j in range(2 * int(eps) + 1)])
                    for i in range(timestamps_1[0], timestamps_1[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c1(timestamps_1[0]) == timestamps_1[0] - 1,
                    c1(timestamps_1[-1]) == timestamps_1[-1] + 1,
                )
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i <= j, And(c0(i) <= c0(j), c1(i) <= c1(j)))
                            for j in range(timestamps_0[0], timestamps_0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps_0[0], timestamps_0[-1] + 1)
                ]
            )
        )

        # violation check (until)
        v = Int("v")
        s.add(And(v >= timestamps_0[0], v <= timestamps_0[-1]))

        u = Int("u")
        s.add(And(u >= timestamps_0[0], u <= v))

        s.add(
            ForAll(
                u,
                Implies(
                    And(
                        u >= timestamps_0[0],
                        u <= v,
                        v >= timestamps_0[0],
                        v <= timestamps_0[-1],
                    ),
                    And(z3Interpolate(c_flow, u) >= 1, z3Interpolate(c_flow, v) == 2),
                ),
            )
        )

        if s.check() == sat:

            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            # print(m)
            print("sat in segment", i)

        elif i <= segCount:

            for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

                if j >= 0 and j < len(data_0) and data_0[j][1] == 0:

                    print("unsat in segment", i)
                    return

            print("unknown in segment", i)

        s.reset()


def z3Interpolate(f, p):

    # return f(ToInt(p)) + ((f(ToInt(p) + 1) - f(ToInt(p))) * (p - ToInt(p)))
    return f(p)


def z3Abs(x):

    return If(x >= 0, x, -x)


def z3SqDist3d(x1, x2, y1, y2, z1, z2):

    return ((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)) + ((z2 - z1) * (z2 - z1))


def z3SqDist2d(x1, x2, y1, y2):

    return ((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1))


def z3SqDist1d(x1, x2):

    return z3Abs(x2 - x1)


def getDataTest(agent_ID):

    file = open("test{}.txt".format(agent_ID))
    line = file.readline()

    data = []

    while line:

        # param = line.split("\t")
        param = line.split(" ")

        values = []

        for i in range(2):

            values.append(float(param[i].strip()))

        data.append(values)

        line = file.readline()

    file.close()

    # print(data)

    return data

def getData(d, setID):
    # file = open('/home/ege/Desktop/repos/distributed-monitoring/code/smt/dataNew/{}_{}.txt'.format(d, setID))
    file = open("dataNew/{}_{}.txt".format(d, setID))
    # file = open(
    #     "{}\\dataNew\\{}_{}.txt".format(
    #         os.path.dirname(os.path.abspath(__file__)), d, setID
    #     )
    # )
    line = file.readline()

    data = []

    while line:

        # param = line.split("\t")
        param = line.split(" ")

        values = []

        for i in range(2):

            values.append(float(param[i].strip()))

        data.append(values)

        line = file.readline()

    file.close()

    # print(data)

    return data

def preprocess(data, d):
    out = []
    for i in range(d):
        if data[i][1] > 0:
            out.append([data[i][0], 1.0])
        else:
            out.append([data[i][0], 0.0])

    out.append([float(d), data[d-1][1]])

    return out

def negate(data):
    out = []
    for i in range(len(data)):
        out.append([data[i][0], 1.0 - data[i][1]])

    return out

def main():
    # set repeat count for confidence interval
    repeat = 1

    # for d in range(32,33):
    #     for eps in (1, 2, 4, 8):
    #         if d < eps:
    #             continue
            
    #         for c in range(100):
    #             flag = False
    #             flagneg = False
    #             out = "0"
    #             outneg = "0"
    #             data_0 = getData(d, c)
    #             data_1 = getData(d, c + 100)

    #             prep_time = 0
    #             eval_time = 0
    #             neg_time = 0
    #             for i in range(repeat):
    #                 t0 = time.time()
    #                 data0 = preprocess(data_0, d)
    #                 data1 = preprocess(data_1, d)
    #                 t1 = time.time()
    #                 flag = prog_always_conjunction(eps, 1, data0, data1)
    #                 t2 = time.time()
    #                 flagneg = prog_eventually_disjunction(eps, 1, negate(data0), negate(data1))
    #                 t3 = time.time()

    #                 prep_time += t1 - t0 
    #                 eval_time += t2 - t1 
    #                 neg_time += t3 - t2

    #             if (flag):
    #                 out = "1"
    #             if (flagneg):
    #                 outneg = "1"

    #             line = str(d) + " " + str(eps) + " " + "-" + " " + str(c) + " "  + "-" + " " + str((prep_time + eval_time) / repeat) + " " + str((prep_time + neg_time) / repeat) + " " + out + " " + outneg
    #             print("AC " + line)
    #             results = open("results_ac_smt+neg.txt", "a")
    #             results.write(line + "\n")
    #             results.close()

    for d in range(4, 8, 16):
        for eps in (1, 2, 4, 8):
            if d < eps:
                continue
            
            for c in range(100):
                flag = False
                flagneg = False
                out = "0"
                outneg = "0"
                data_0 = getData(d, c)
                data_1 = getData(d, c + 100)
                data_2 = getData(32, c)
                data_3 = getData(32, c + 100)

                prep_time = 0
                eval_time = 0
                neg_time = 0
                for i in range(repeat):
                    t0 = time.time()
                    data0 = preprocess(data_0, d)
                    data1 = preprocess(data_1, d)
                    data2 = preprocess(data_2, d)
                    data3 = preprocess(data_3, d)
                    t1 = time.time()
                    flag = prog_always_disjunction4(eps, 1, data0, data1, data2, data3)
                    t2 = time.time()
                    flagneg = prog_eventually_conjunction4(eps, 1, negate(data0), negate(data1), negate(data2), negate(data3))
                    t3 = time.time()

                    prep_time += t1 - t0 
                    eval_time += t2 - t1 
                    neg_time += t3 - t2

                if (flag):
                    out = "1"
                if (flagneg):
                    outneg = "1"
            
                line = str(d) + " " + str(eps) + " " + "-" + " " + str(c) + " "  + "-" + " " + str((prep_time + eval_time) / repeat) + " " + str((prep_time + neg_time) / repeat) + " " + out + " " + outneg
                print("AD " + line)
                results = open("results_ad_TEST.txt", "a")
                # results = open("results_ad_smt+neg.txt", "a")
                results.write(line + "\n")
                results.close()

    # for d in range(32,33):
    #     for eps in (4, 8):
    #         if d < eps:
    #             continue
            
    #         for c in range(100):
    #             flag = False
    #             flagneg = False
    #             out = "0"
    #             outneg = "0"
    #             data_0 = getData(d, c)
    #             data_1 = getData(d, c + 100)

    #             prep_time = 0
    #             eval_time = 0
    #             neg_time = 0
    #             for i in range(repeat):
    #                 t0 = time.time()
    #                 data0 = preprocess(data_0, d)
    #                 data1 = preprocess(data_1, d)
    #                 t1 = time.time()
    #                 flag = prog_eventually_conjunction(eps, 1, data0, data1)
    #                 t2 = time.time()
    #                 flagneg = prog_always_disjunction(eps, 1, negate(data0), negate(data1))
    #                 t3 = time.time()

    #                 prep_time += t1 - t0 
    #                 eval_time += t2 - t1 
    #                 neg_time += t3 - t2

    #             if (flag):
    #                 out = "1"
    #             if (flagneg):
    #                 outneg = "1"

    #             line = str(d) + " " + str(eps) + " " + "-" + " " + str(c) + " "  + "-" + " " + str((prep_time + eval_time) / repeat) + " " + str((prep_time + neg_time) / repeat) + " " + out + " " + outneg
    #             print("EC " + line)
    #             results = open("results_ec_smt+neg.txt", "a")
    #             results.write(line + "\n")
    #             results.close()

    # for d in range(32,33):
    #     for eps in (1, 2, 4, 8):
    #         if d < eps:
    #             continue
            
    #         for c in range(100):
    #             flag = False
    #             flagneg = False
    #             out = "0"
    #             outneg = "0"
    #             data_0 = getData(d, c)
    #             data_1 = getData(d, c + 100)

    #             prep_time = 0
    #             eval_time = 0
    #             neg_time = 0
    #             for i in range(repeat):
    #                 t0 = time.time()
    #                 data0 = preprocess(data_0, d)
    #                 data1 = preprocess(data_1, d)
    #                 t1 = time.time()
    #                 flag = prog_eventually_disjunction(eps, 1, data0, data1)
    #                 t2 = time.time()
    #                 flagneg = prog_always_conjunction(eps, 1, negate(data0), negate(data1))
    #                 t3 = time.time()

    #                 prep_time += t1 - t0 
    #                 eval_time += t2 - t1 
    #                 neg_time += t3 - t2
                
    #             if (flag):
    #                 out = "1"
    #             if (flagneg):
    #                 outneg = "1"

    #             line = str(d) + " " + str(eps) + " " + "-" + " " + str(c) + " "  + "-" + " " + str((prep_time + eval_time) / repeat) + " " + str((prep_time + neg_time) / repeat) + " " + out + " " + outneg
    #             print("ED" + line)
    #             results = open("results_ed_smt+neg.txt", "a")
    #             results.write(line + "\n")
    #             results.close()

if __name__ == "__main__":
    main()
    pass