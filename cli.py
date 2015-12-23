import sys

def parse(args,keys):
	ii=0
	values=keys

	while ii<len(args):
		if args[ii] in keys:
			if keys[args[ii]]:
				if ii+1<len(args):
					values[args[ii]]=args[ii+1]
					ii+=1

				else:
					raise Exception('Expected value after option "'+args[ii]+'".')

			else:
				values[args[ii]]=True

		else:
			raise Exception('Unknown command line option "'+args[ii]+'".')

		ii+=1

	return values