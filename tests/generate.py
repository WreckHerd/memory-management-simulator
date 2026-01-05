import random
import sys

def generate_stress_test():
    filename = "tests/trace_stress_test.txt"
    commands = []

    # --- CONFIGURATION ---
    # 1. Total Memory: 16384 bytes (16KB)
    #    Small enough that 500 ops will definitely fill it up.
    MEM_SIZE = 16384
    
    # 2. Allocation Strategy
    #    CHANGE THIS MANUALLY in the generated file later to test others
    STRATEGY = "bestfit" 

    # Header Inputs
    commands.append(str(MEM_SIZE))
    commands.append(STRATEGY)
    commands.append("256")    # L1 Size
    commands.append("1024")   # L2 Size
    commands.append("32")     # Line Size
    commands.append("4")      # Associativity
    commands.append("lru")    # Replacement Policy

    # --- SIMULATION STATE ---
    active_ids = []
    next_id = 1
    
    # Operations Count
    TOTAL_OPS = 600 

    for i in range(TOTAL_OPS):
        # Logic: 
        # 1. Early phase: Mostly allocate to fill memory.
        # 2. Middle phase: High churn (alloc/free mix).
        # 3. Late phase: Try to allocate LARGE blocks that require coalescing.

        mode = "alloc"
        
        # If we have too many active blocks (>50), force some frees to create fragmentation
        if len(active_ids) > 50:
            mode = "free"
        # If we have very few, force alloc
        elif len(active_ids) < 10:
            mode = "alloc"
        else:
            # Random mix
            mode = "alloc" if random.random() > 0.4 else "free"

        if mode == "free" and active_ids:
            # FREE
            # Pick a random victim
            victim = random.choice(active_ids)
            commands.append(f"free {victim}")
            active_ids.remove(victim)
        else:
            # ALLOC
            # Generate sizes that are "awkward" for the allocator
            # Powers of 2 are too easy. We want 300, 750, 1500 etc.
            
            # 10% chance of a "Big Chunk" (forces failure if fragmented)
            if random.random() < 0.1:
                size = random.randint(2000, 4000)
            else:
                # Standard chunk
                size = random.randint(50, 600)
                
            commands.append(f"malloc {size}")
            # We assume it succeeds for ID tracking (the simulator handles failures)
            active_ids.append(next_id)
            next_id += 1

    commands.append("stats")
    commands.append("exit")

    with open(filename, "w") as f:
        f.write("\n".join(commands))
    
    print(f"Generated {filename} with {len(commands)} operations.")

if __name__ == "__main__":
    generate_stress_test()