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

    # v_0 = 0
    # for i in range(1, len(data_0)):
    #     if (data_0[i][1] != data_0[i - 1][1]):
    #         v_0 += 1

    # v_1 = 0
    # for i in range(1, len(data_1)):
    #     if (data_1[i][1] != data_1[i - 1][1]):
    #         v_1 += 1

    pad = min(2 * eps, len(data_0) - 1)
    # pad = max(1, min(2 * eps, len(data_0) - 1) - min(v_0, v_1))
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
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []
        var_0 = [0]

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0) - 1:

                s.add(sig0(j) == data_0[j][1])
                for k in range(pad):
                    timestamps0.append(j * pad + k)

                if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                    var_0.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (var_0[-1] < segmentUpperBound):
            var_0.append(segmentUpperBound)            

        timestamps1 = []
        var_1 = [0]
        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1) - 1:

                s.add(sig1(j) == data_1[j][1])
                for k in range(pad):
                    timestamps1.append(j * pad + k)

                if (j > 0 and data_1[j][1] != data_1[j - 1][1]):
                    var_1.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (var_1[-1] < segmentUpperBound):
            var_1.append(segmentUpperBound)            

        if not entryFound:

            ##print()
            pass

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c0(i) == (min(timestamps0[-1]//pad, (max(0, ((i//pad - eps) + j))))) for j in range(1, 2 * int(eps))])
                    # Or([c0(i) == (min(timestamps0[-1], (max(0, ((i - peps) + j))))) for j in range(0, 2 * int(peps) + 1)])
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c1(i) == (min(timestamps1[-1]//pad, (max(0, ((i//pad - eps) + j))))) for j in range(1, 2 * int(eps))])
                    # Or([c1(i) == (min(timestamps1[-1], (max(0, ((i - peps) + j))))) for j in range(0, 2 * int(peps) + 1)])
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        
        # retiming preserves signal variability
        s.add(
            And(
                [
                    Or(
                        [
                            And(var_0[i] <= c0(j), c0(j) < var_0[i + 1])
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(len(var_0) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(var_1[i] <= c1(j), c1(j) < var_1[i + 1])
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(len(var_1) - 1)
                ]
            )
        )

        # x0 = Function("x0", IntSort(), IntSort())
        # s.add(x0(0) == 0)
        # s.add(
        #     And(
        #         [
        #             x0(i) == x0(i - 1) + ((sig0(c0(i - 1)) + sig0(c0(i))) % 2)
        #             for i in range(timestamps0[0] + 1, timestamps0[-1] + 1)
        #         ]
        #     )
        # )
        # s.add(x0(timestamps0[-1]) == var_0)

        # x1 = Function("x1", IntSort(), IntSort())
        # s.add(x1(0) == 0)
        # s.add(
        #     And(
        #         [
        #             x1(i) == x1(i - 1) + ((sig1(c1(i - 1)) + sig1(c1(i))) % 2)
        #             for i in range(timestamps1[0] + 1, timestamps1[-1] + 1)
        #         ]
        #     )
        # )
        # s.add(x1(timestamps1[-1]) == var_1)

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) < eps, c0(i) - c1(i) > -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
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
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    And(
                        [
                            Implies(j - i >= peps, And(c0(i) < c0(j), c1(i) < c1(j), c0(i) < c1(j), c1(i) < c0(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
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
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps0[0], v <= timestamps0[-1]))

        s.add(
            Implies(
                And(v >= timestamps0[0], v <= timestamps0[-1]),
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



def prog_always_disjunction(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # v_0 = 0
    # for i in range(1, len(data_0)):
    #     if (data_0[i][1] != data_0[i - 1][1]):
    #         v_0 += 1

    # v_1 = 0
    # for i in range(1, len(data_1)):
    #     if (data_1[i][1] != data_1[i - 1][1]):
    #         v_1 += 1

    pad = min(2 * eps, len(data_0) - 1)
    # pad = max(1, min(2 * eps, len(data_0) - 1) - min(v_0, v_1))
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
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []
        var_0 = [0]

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0) - 1:

                for k in range(pad):
                    s.add(sig0(j * pad + k) == data_0[j][1])
                    timestamps0.append(j * pad + k)

                if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                    var_0.append(j * pad)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (var_0[-1] < segmentUpperBound * pad):
            var_0.append(segmentUpperBound * pad)            

        timestamps1 = []
        var_1 = [0]
        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1) - 1:

                for k in range(pad):
                    s.add(sig1(j * pad + k) == data_1[j][1])
                    timestamps1.append(j * pad + k)

                if (j > 0 and data_1[j][1] != data_1[j - 1][1]):
                    var_1.append(j * pad)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (var_1[-1] < segmentUpperBound * pad):
            var_1.append(segmentUpperBound * pad)            

        if not entryFound:

            ##print()
            pass

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c0(i) == (min(timestamps0[-1], (max(0, ((i - peps) + j))))) for j in range(1, 2 * int(peps))])
                    # Or([c0(i) == (min(timestamps0[-1], (max(0, ((i - peps) + j))))) for j in range(0, 2 * int(peps) + 1)])
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c1(i) == (min(timestamps1[-1], (max(0, ((i - peps) + j))))) for j in range(1, 2 * int(peps))])
                    # Or([c1(i) == (min(timestamps1[-1], (max(0, ((i - peps) + j))))) for j in range(0, 2 * int(peps) + 1)])
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        
        # retiming preserves signal variability
        s.add(
            And(
                [
                    Or(
                        [
                            And(var_0[i] <= c0(j), c0(j) < var_0[i + 1])
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(len(var_0) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(var_1[i] <= c1(j), c1(j) < var_1[i + 1])
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(len(var_1) - 1)
                ]
            )
        )

        # x0 = Function("x0", IntSort(), IntSort())
        # s.add(x0(0) == 0)
        # s.add(
        #     And(
        #         [
        #             x0(i) == x0(i - 1) + ((sig0(c0(i - 1)) + sig0(c0(i))) % 2)
        #             for i in range(timestamps0[0] + 1, timestamps0[-1] + 1)
        #         ]
        #     )
        # )
        # s.add(x0(timestamps0[-1]) == var_0)

        # x1 = Function("x1", IntSort(), IntSort())
        # s.add(x1(0) == 0)
        # s.add(
        #     And(
        #         [
        #             x1(i) == x1(i - 1) + ((sig1(c1(i - 1)) + sig1(c1(i))) % 2)
        #             for i in range(timestamps1[0] + 1, timestamps1[-1] + 1)
        #         ]
        #     )
        # )
        # s.add(x1(timestamps1[-1]) == var_1)

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) < peps, c0(i) - c1(i) > -peps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
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
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # s.add(
        #     And(
        #         [
        #             And(
        #                 [
        #                     Implies(j - i >= peps, And(c0(i) < c0(j), c1(i) < c1(j), c0(i) < c1(j), c1(i) < c0(j)))
        #                     for j in range(timestamps0[0], timestamps0[-1] + 1)
        #                 ]
        #             )
        #             for i in range(timestamps0[0], timestamps0[-1] + 1)
        #         ]
        #     )
        # )

        # consistent cut flow
        # _flow = Function("c_flow", IntSort(), RealSort())
        c_flow = Function("c_flow", IntSort(), IntSort())

        # addition
        s.add(
            And(
                [
                    c_flow(i) == (sig0(c0(i)) + sig1(c1(i)))
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )
        # violation check
        v = Int("v")
        s.add(And(v >= timestamps0[0], v <= timestamps0[-1]))

        s.add(
            Implies(
                And(v >= timestamps0[0], v <= timestamps0[-1]),
                z3Interpolate(c_flow, v) == 0
            )
        )
        
        print(s.assertions())
        if s.check() == sat:

            m = s.model()
            # out = "%s %s" % (m[test], m[test2])
            print(m)
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

    # v_0 = 0
    # for i in range(1, len(data_0)):
    #     if (data_0[i][1] != data_0[i - 1][1]):
    #         v_0 += 1

    # v_1 = 0
    # for i in range(1, len(data_1)):
    #     if (data_1[i][1] != data_1[i - 1][1]):
    #         v_1 += 1

    pad = min(2 * eps, len(data_0) - 1)
    # pad = max(1, min(2 * eps, len(data_0) - 1) - min(v_0, v_1))
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
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []
        var_0 = [0]

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0) - 1:

                for k in range(pad):
                    s.add(sig0(j * pad + k) == data_0[j][1])
                    timestamps0.append(j * pad + k)

                if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                    var_0.append(j * pad)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (var_0[-1] < segmentUpperBound * pad):
            var_0.append(segmentUpperBound * pad)            

        timestamps1 = []
        var_1 = [0]
        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1) - 1:

                for k in range(pad):
                    s.add(sig1(j * pad + k) == data_1[j][1])
                    timestamps1.append(j * pad + k)

                if (j > 0 and data_1[j][1] != data_1[j - 1][1]):
                    var_1.append(j * pad)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (var_1[-1] < segmentUpperBound * pad):
            var_1.append(segmentUpperBound * pad)            

        if not entryFound:

            ##print()
            pass

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c0(i) == (min(timestamps0[-1], (max(0, ((i - eps) + j))))) for j in range(1, 2 * int(peps))])
                    # Or([c0(i) == (min(timestamps0[-1], (max(0, ((i - peps) + j))))) for j in range(0, 2 * int(peps) + 1)])
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c1(i) == (min(timestamps1[-1], (max(0, ((i - eps) + j))))) for j in range(1, 2 * int(peps))])
                    # Or([c1(i) == (min(timestamps1[-1], (max(0, ((i - peps) + j))))) for j in range(0, 2 * int(peps) + 1)])
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        
        # retiming preserves signal variability
        s.add(
            And(
                [
                    Or(
                        [
                            And(var_0[i] <= c0(j), c0(j) < var_0[i + 1])
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(len(var_0) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(var_1[i] <= c1(j), c1(j) < var_1[i + 1])
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(len(var_1) - 1)
                ]
            )
        )

        # x0 = Function("x0", IntSort(), IntSort())
        # s.add(x0(0) == 0)
        # s.add(
        #     And(
        #         [
        #             x0(i) == x0(i - 1) + ((sig0(c0(i - 1)) + sig0(c0(i))) % 2)
        #             for i in range(timestamps0[0] + 1, timestamps0[-1] + 1)
        #         ]
        #     )
        # )
        # s.add(x0(timestamps0[-1]) == var_0)

        # x1 = Function("x1", IntSort(), IntSort())
        # s.add(x1(0) == 0)
        # s.add(
        #     And(
        #         [
        #             x1(i) == x1(i - 1) + ((sig1(c1(i - 1)) + sig1(c1(i))) % 2)
        #             for i in range(timestamps1[0] + 1, timestamps1[-1] + 1)
        #         ]
        #     )
        # )
        # s.add(x1(timestamps1[-1]) == var_1)

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) < peps, c0(i) - c1(i) > -peps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # global clock to local clock mappings are ordered
        s.add(
            And(
                [
                    And(
                        [
                            Implies(i < j, And(c0(i) < c0(j), c1(i) < c1(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    And(
                        [
                            Implies(And(j % pad == 0, i % pad == 0, j - i >= peps), And(c0(i) < c0(j), c1(i) < c1(j), c0(i) < c1(j), c1(i) < c0(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    And(
                        [
                            Implies(And(Or(j % pad != 0, i % pad != 0), j - i > peps), And(c0(i) < c0(j), c1(i) < c1(j), c0(i) < c1(j), c1(i) < c0(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
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
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps0[0], v <= timestamps0[-1]))

        s.add(
            ForAll(v,
                Implies(
                    And(v >= timestamps0[0], v <= timestamps0[-1]),
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

            # terminate after unsat
            return flag

        elif i <= segCount:

            ##print("sat in segment", i)
            flag = True


        s.reset()
        
    return flag


def prog_eventually_disjunction(eps, segCount, data_0, data_1):

    # initialize z3 solver
    s = Solver()

    # v_0 = 0
    # for i in range(1, len(data_0)):
    #     if (data_0[i][1] != data_0[i - 1][1]):
    #         v_0 += 1

    # v_1 = 0
    # for i in range(1, len(data_1)):
    #     if (data_1[i][1] != data_1[i - 1][1]):
    #         v_1 += 1

    pad = min(2 * eps, len(data_0) - 1)
    # pad = max(1, min(2 * eps, len(data_0) - 1) - min(v_0, v_1))
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
        segmentLowerBound = int((i * segDur) - eps)
        segmentUpperBound = int((i + 1) * segDur)

        timestamps0 = []
        var_0 = [0]

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0) - 1:

                s.add(sig0(j) == data_0[j][1])
                for k in range(pad):
                    timestamps0.append(j * pad + k)

                if (j > 0 and data_0[j][1] != data_0[j - 1][1]):
                    var_0.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (var_0[-1] < segmentUpperBound):
            var_0.append(segmentUpperBound)            

        timestamps1 = []
        var_1 = [0]
        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1) - 1:

                s.add(sig1(j) == data_1[j][1])
                for k in range(pad):
                    timestamps1.append(j * pad + k)

                if (j > 0 and data_1[j][1] != data_1[j - 1][1]):
                    var_1.append(j)

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        if (var_1[-1] < segmentUpperBound):
            var_1.append(segmentUpperBound)            

        if not entryFound:

            ##print()
            pass

        i += 1

        # force terminate after one loop
        # entryFound = False

        # global clock to local clock mappings
        c0 = Function("c0", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c0(i) == (min(timestamps0[-1]//pad, (max(0, ((i//pad - eps) + j))))) for j in range(1, 2 * int(eps))])
                    # Or([c0(i) == (min(timestamps0[-1], (max(0, ((i - peps) + j))))) for j in range(0, 2 * int(peps) + 1)])
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c1(i) == (min(timestamps1[-1]//pad, (max(0, ((i//pad - eps) + j))))) for j in range(1, 2 * int(eps))])
                    # Or([c1(i) == (min(timestamps1[-1], (max(0, ((i - peps) + j))))) for j in range(0, 2 * int(peps) + 1)])
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        
        # retiming preserves signal variability
        s.add(
            And(
                [
                    Or(
                        [
                            And(var_0[i] <= c0(j), c0(j) < var_0[i + 1])
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(len(var_0) - 1)
                ]
            )
        )

        s.add(
            And(
                [
                    Or(
                        [
                            And(var_1[i] <= c1(j), c1(j) < var_1[i + 1])
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(len(var_1) - 1)
                ]
            )
        )

        # x0 = Function("x0", IntSort(), IntSort())
        # s.add(x0(0) == 0)
        # s.add(
        #     And(
        #         [
        #             x0(i) == x0(i - 1) + ((sig0(c0(i - 1)) + sig0(c0(i))) % 2)
        #             for i in range(timestamps0[0] + 1, timestamps0[-1] + 1)
        #         ]
        #     )
        # )
        # s.add(x0(timestamps0[-1]) == var_0)

        # x1 = Function("x1", IntSort(), IntSort())
        # s.add(x1(0) == 0)
        # s.add(
        #     And(
        #         [
        #             x1(i) == x1(i - 1) + ((sig1(c1(i - 1)) + sig1(c1(i))) % 2)
        #             for i in range(timestamps1[0] + 1, timestamps1[-1] + 1)
        #         ]
        #     )
        # )
        # s.add(x1(timestamps1[-1]) == var_1)

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) < eps, c0(i) - c1(i) > -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
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
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        s.add(
            And(
                [
                    And(
                        [
                            Implies(j - i >= peps, And(c0(i) < c0(j), c1(i) < c1(j), c0(i) < c1(j), c1(i) < c0(j)))
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
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
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check
        v = Int("v")
        s.add(And(v >= timestamps0[0], v <= timestamps0[-1]))

        s.add(
            ForAll(v,
                Implies(
                    And(v >= timestamps0[0], v <= timestamps0[-1]),
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

            # terminate after unsat
            return flag

        elif i <= segCount:

            ##print("sat in segment", i)
            flag = True

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

        timestamps0 = []

        # sig0 = Function("sig0", IntSort(), RealSort())
        sig0 = Function("sig0", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_0):

                timestamps0.append(j)

                s.add(sig0(j) == data_0[j][1])

                if j > (i * segDur):

                    entryFound = True

                if j == sigDur:

                    entryFound = False

        timestamps1 = []

        # sig1 = Function("sig1", IntSort(), RealSort())
        sig1 = Function("sig1", IntSort(), IntSort())

        for j in range((segmentLowerBound + 0), (segmentUpperBound + 1)):

            if j >= 0 and j < len(data_1):

                timestamps1.append(j)

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
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c0(timestamps0[0]) == timestamps0[0] - 1,
                    c0(timestamps0[-1]) == timestamps0[-1] + 1,
                )
            )
        )

        c1 = Function("c1", IntSort(), IntSort())
        s.add(
            And(
                [
                    Or([c1(i) == ((i - eps) + j) for j in range(2 * int(eps) + 1)])
                    for i in range(timestamps1[0], timestamps1[-1] + 1)
                ]
            )
        )
        s.add(
            Not(
                Or(
                    c1(timestamps1[0]) == timestamps1[0] - 1,
                    c1(timestamps1[-1]) == timestamps1[-1] + 1,
                )
            )
        )

        # local clocks are bound by epsilon
        s.add(
            And(
                [
                    And(c0(i) - c1(i) <= eps, c0(i) - c1(i) >= -eps)
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
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
                            for j in range(timestamps0[0], timestamps0[-1] + 1)
                        ]
                    )
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
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
                    for i in range(timestamps0[0], timestamps0[-1] + 1)
                ]
            )
        )

        # violation check (until)
        v = Int("v")
        s.add(And(v >= timestamps0[0], v <= timestamps0[-1]))

        u = Int("u")
        s.add(And(u >= timestamps0[0], u <= v))

        s.add(
            ForAll(
                u,
                Implies(
                    And(
                        u >= timestamps0[0],
                        u <= v,
                        v >= timestamps0[0],
                        v <= timestamps0[-1],
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
    for i in range(d):
        if data[i][1] > 0:
            data[i][1] = 1.0
        else:
            data[i][1] = 0.0

    data.append([float(d), data[d-1][1]])

    return data

def negate(data):
    for i in range(len(data)):
        data[i][1] = 1.0 - data[i][1]

    return data

def main():
    # set repeat count for confidence interval
    repeat = 1

    for d in (4, 8, 16, 32):
        for eps in (1, 2, 4, 8):
            if d < eps:
                continue
            
            for c in range(13,100):
                flag = False
                flagneg = False
                out = "0"
                outneg = "0"
                # data_0 = getDataTest(1)
                # data_1 = getDataTest(2)
                data_0 = getData(d, c)
                data_1 = getData(d, c + 100)

                prep_time = 0
                eval_time = 0
                neg_time = 0
                for i in range(repeat):
                    t0 = time.time()
                    data0 = preprocess(data_0, d)
                    data1 = preprocess(data_1, d)
                    t1 = time.time()
                    # flag = prog_always_conjunction(eps, 1, data0, data1) # d / min(d, 8)
                    flag = prog_always_disjunction(eps, 1, data0, data1)
                    # flag = prog_eventually_conjunction(eps, 1, data0, data1)
                    # flag = prog_eventually_disjunction(eps, 1, data0, data1)
                    t2 = time.time()
                    # flagneg = prog_eventually_disjunction(eps, 1, negate(data0), negate(data1))
                    flagneg = prog_eventually_conjunction(eps, 1, negate(data0), negate(data1))
                    # flagneg = prog_always_disjunction(eps, 1, negate(data0), negate(data1))
                    # flagneg = prog_always_conjunction(eps, 1, negate(data0), negate(data1))
                    t3 = time.time()

                    prep_time += t1 - t0 
                    eval_time += t2 - t1 
                    neg_time += t3 - t2

                
                if (flag):
                    out = "1"
                if (flagneg):
                    outneg = "1"
            

                line = str(d) + " " + str(eps) + " " + "-" + " " + str(c) + " "  + "-" + " " + str((prep_time + eval_time) / repeat) + " " + str((prep_time + neg_time) / repeat) + " " + out + " " + outneg
                print(line)
                results = open("results_ad_smt+negggTESTT.txt", "a")
                results.write(line + "\n")
                results.close()

if __name__ == "__main__":
    main()
    pass
