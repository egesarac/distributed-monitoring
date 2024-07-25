First, a word on terminology: this is not distributed monitoring. This is centralized monitoring of a distributed systems. Please update the writing, and the title, accordingly.
	describe the setup once in the intro, say we refer to this as distributed monitoring

Line 499: you claim that the focus of the paper is on offline  monitoring, then why do you care about resources? Why trace  prediction for efficiency? The combination of approximate and  precise to get a faster precise when ? is produce is a neat idea  though. 
	make it more explicit in the intro

I am very confused by the mention of a “decentralized setting” at line 460: the monitor as given above is centralized. What is the decentralized version? This is the first time that the word decentralized appears in the paper.
	introduce decenteralized monitoring better, make it more prominent

========

The writing is unnecessarily squeezed. Lots of negative spaces,  small fonts in the math equations, unreadable figures, wrap  subfigures that affect the text, etc. All these affects the  readability of the paper. Please do *not* do that. Cut text to tell  a story in the space provided: the presentation must honor the  content. Fig 4, 5 and 6 are unreadable. 
	try to cut some text

Thm 2 (line 419): do not use "resp."
	maybe rephrase in 3 lines? (if enough space)

========

Line 510: Experiment 2 indicates a sampling of 20Hz. With clocksynchronization to 1-10ms using NTP (much better using GPS) one  could achieve a neglible skew.  
	added a sentence: We should say the actual values don’t matter. What matters is when clock skew is larger than sampling we end up having the same problem. There are always applications that sampling is faster than NTP epsilon.

A technical issue is to be found in fig 2c: in the interval [0,1] the only values I see are {0,01}. Why isn’t it possible that the signal is already at 1, so the value expressions for this interval are {0,01,1}? The rise is instantaneous (for a PWC signal)  so it seems entirely possible that within [0,1] the signal rose and is at 1. So {0,01,1} expresses that the transition is complete in the interval. If this observation is correct, I’m not confident that Eq(1) gives the right definition for Vi. Now this observation of mine does not fundamentally invalidate the approach (the value expressions are just more numerous) but it does increase the conservatism of the monitor potentially, yielding a much greater number of inconclusive verdicts.
	added as an assumption: no edges at time 0

The combination of exact and approximate verdicts feels rather ad hoc and its actual performance will obviously depend on the particular signal (how often the approximate computation is inconclusive), so it can be left out. Similarly for the paragraph starting on line 447, it feels ad hoc, and can be removed in an already dense paper. They don't add any new insight and feel like filler. The space can be used for more examples, say, or more experiments. Or it can be used to relax the presentation a little bit, make some figures larger, etc. The presentation right now is very tight.
	kept this part

Line 338 says “that We remark that the definitions of the semantics coincide  with the robustness semantics of STL”: these are max and min over booleans, so the robust semantics actually coincide with the boolean semantics. This remark can therefore be misleading.
Line 338: "We remark that the definitions of the semantics coincidewith the robustness semantics of STL". I don't think this claim istrue. You do not even compute a numerical outcome. In fact, thiswould be a neat idea (calculate robustness values, precise or over-approximate) for a signal and its negation.
	removed this sentence

The paragraph on timed operations (page 10) could really use a figure.
	fig 3 is for this part
	can't move it up (a lot of small math/example blocks), but highlighted at the end of the first paragraph

