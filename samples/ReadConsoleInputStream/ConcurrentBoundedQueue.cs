using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using Microsoft.Extensions.Logging;

namespace Samples.Terminal
{
    /// <summary>
    /// Implements a bounded queue that won't block on overflow; instead the oldest item is discarded.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class ConcurrentBoundedQueue<T> : ConcurrentQueue<T>
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public ConcurrentBoundedQueue(int capacity)
        {
			_logger.LogDebug(">>> Entering ConcurrentBoundedQueue(capacity={Capacity})", capacity);
            Capacity = GetAlignedCapacity(capacity);
			_logger.LogDebug("ConcurrentBoundedQueue(collection={Collection},capacity={Capacity}): Capacity → {Capacity1}", collection, capacity, Capacity);
			_logger.LogDebug("<<< Exiting ConcurrentBoundedQueue(capacity={Capacity})", capacity);
			_logger.LogDebug("ConcurrentBoundedQueue(capacity={Capacity}): Capacity → {Capacity1}", capacity, Capacity);
        }

        public ConcurrentBoundedQueue(IEnumerable<T> collection, int capacity) : base(collection)
        {
			_logger.LogDebug(">>> Entering ConcurrentBoundedQueue(collection={Collection},capacity={Capacity})", collection, capacity);
            Capacity = GetAlignedCapacity(capacity);
        }

        private int GetAlignedCapacity(int n)
        {
			_logger.LogDebug(">>> Entering GetAlignedCapacity(n={N})", n);
            if (n < 2)
            {
				_logger.LogWarning("Throwing ArgumentException from GetAlignedCapacity(n={N})", n);
                throw new ArgumentException("Capacity must be at least 2");
            }

            var f = Math.Log(n, 2);
			_logger.LogDebug("GetAlignedCapacity(n={N}): f → {F}", n, f);
            var p = Math.Ceiling(f);
			_logger.LogDebug("GetAlignedCapacity(n={N}): p → {P}", n, p);

			_logger.LogDebug("<<< Exiting GetAlignedCapacity(n={N})", n);
            return (int) Math.Pow(2, p);
        }

        public new void Enqueue(T item)
        {
			_logger.LogDebug(">>> Entering Enqueue(item={Item})", item);
            // if we're about to overflow, dump oldest item
            if (Count >= Capacity)
            {
                lock (this)
                {
                    while (Count >= Capacity)
                    {
                        TryDequeue(out _);
                    }
                }
            }

            base.Enqueue(item);
			_logger.LogDebug("<<< Exiting Enqueue(item={Item})", item);
        }

        public int Capacity
        {
            get; private set;
        }
    }
}
