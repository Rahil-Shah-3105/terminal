#include <stdio.h>
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

namespace winrt::Microsoft::Terminal::TerminalConnection::implementation
{
    template<typename T>
    struct BaseTerminalConnection
    {
    public:
        winrt::guid SessionId() const noexcept
        {
			fprintf(stderr, ">>> Entering SessionId()\n");
            fprintf(stderr, "<<< Exiting SessionId()\n");
            return _sessionId;
        }

        ConnectionState State() const noexcept
        {
			fprintf(stderr, ">>> Entering State()\n");
            fprintf(stderr, "<<< Exiting State()\n");
            return _connectionState;
        }

        til::typed_event<ITerminalConnection, winrt::Windows::Foundation::IInspectable> StateChanged;

    protected:
        template<typename U>
        U unbox_prop_or(const Windows::Foundation::Collections::ValueSet& blob, std::wstring_view key, U defaultValue)
        {
			fprintf(stderr, ">>> Entering unbox_prop_or(blob=%d,key=%d,defaultValue=%d)\n", blob, key, defaultValue);
            fprintf(stderr, "<<< Exiting unbox_prop_or(blob=%d,key=%d,defaultValue=%d)\n", blob, key, defaultValue);
            return winrt::unbox_value_or<U>(blob.TryLookup(key).try_as<Windows::Foundation::IPropertyValue>(), defaultValue);
        }

#pragma warning(push)
#pragma warning(disable : 26447) // Analyzer is still upset about noexcepts throwing even with function level try.
        // Method Description:
        // - Attempt to transition to and signal the specified connection state.
        //   The transition will only be effected if the state is "beyond" the current state.
        // Arguments:
        // - state: the new state
        // Return Value:
        //   Whether we've successfully transitioned to the new state.
        bool _transitionToState(const ConnectionState state) noexcept
        try
        {
			fprintf(stderr, ">>> Entering _transitionToState(state=%d)\n", state);
            {
                std::lock_guard<std::mutex> stateLock{ _stateMutex };
                // only allow movement up the state gradient
                if (state < _connectionState)
                {
                    return false;
                }
                _connectionState = state;
				fprintf(stderr, "_transitionToState(state=%d): _connectionState → %d\n", state, _connectionState);
            }
			fprintf(stderr, "<<< Exiting _transitionToState(state=%d)\n", state);
            // Dispatch the event outside of lock.
#pragma warning(suppress : 26491) // We can't avoid static_cast downcast because this is template magic.
            StateChanged.raise(*static_cast<T*>(this), nullptr);
            return true;
        }
        CATCH_FAIL_FAST()

        // Method Description:
        // - Returns whether the state is one of the N specified states.
        // Arguments:
        // - "...": the states
        // Return Value:
        //   Whether we're in one of the states.
        template<typename... Args>
        bool _isStateOneOf(const Args&&... args) const noexcept
        try
        {
			fprintf(stderr, ">>> Entering _isStateOneOf(args=%d)\n", args);
            // Dark magic! This function uses C++17 fold expressions. These fold expressions roughly expand as follows:
            // (... OP (expression_using_args))
            // into ->
            // expression_using_args[0] OP expression_using_args[1] OP expression_using_args[2] OP (etc.)
            // We use that to first check that All Args types are ConnectionState (type[0] == ConnectionState && type[1] == ConnectionState && etc.)
            // and then later to check that the current state is one of the passed-in ones:
            // (_state == args[0] || _state == args[1] || etc.)
            static_assert((... && std::is_same<Args, ConnectionState>::value), "all queried connection states must be from the ConnectionState enum");
            std::lock_guard<std::mutex> stateLock{ _stateMutex };
			fprintf(stderr, "<<< Exiting _isStateOneOf(args=%d)\n", args);
            return (... || (_connectionState == args));
        }
        CATCH_FAIL_FAST()

        // Method Description:
        // - Returns whether the state has reached or surpassed the specified state.
        // Arguments:
        // - state; the state to check against
        // Return Value:
        //   Whether we're at or beyond the specified state
        bool _isStateAtOrBeyond(const ConnectionState state) const noexcept
        try
        {
			fprintf(stderr, ">>> Entering _isStateAtOrBeyond(state=%d)\n", state);
            std::lock_guard<std::mutex> stateLock{ _stateMutex };
			fprintf(stderr, "<<< Exiting _isStateAtOrBeyond(state=%d)\n", state);
            return _connectionState >= state;
        }
        CATCH_FAIL_FAST()
#pragma warning(pop)

        // Method Description:
        // - (Convenience:) Returns whether we're "connected".
        // Return Value:
        //   Whether we're "connected"
        bool _isConnected() const noexcept
        {
			fprintf(stderr, ">>> Entering _isConnected()\n");
            fprintf(stderr, "<<< Exiting _isConnected()\n");
            return _isStateOneOf(ConnectionState::Connected);
        }

        winrt::guid _sessionId{};

    private:
        std::atomic<ConnectionState> _connectionState{ ConnectionState::NotConnected };
        mutable std::mutex _stateMutex;
    };
}
