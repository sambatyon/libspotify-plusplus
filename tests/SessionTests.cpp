#define BOOST_TEST_MODULE SpotifyppTests

#include <log4cplus/loggingmacros.h>
#include <log4cplus/logger.h>

#include <cstdint>
#include <cstddef>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/threadpool.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>

#include <spotify/Session.hpp>

#include "appkeys.hpp"

namespace {
log4cplus::Logger logger = log4cplus::Logger::getInstance("tests.SessionTests");
}


BOOST_FIXTURE_TEST_SUITE(SessionTests, SpotifyBasicFixture)

BOOST_AUTO_TEST_CASE(TestLogin)
{
    // Use the threadpool to keep all calls to libspotify in a single thread
    boost::threadpool::pool thread_pool(1);
    boost::shared_ptr<spotify::Session> session;
    thread_pool.schedule([&]{session = spotify::Session::Create();});
    while (!session)
        boost::this_thread::sleep(boost::posix_time::millisec(20));
    thread_pool.schedule([&]{
        BOOST_REQUIRE(session->Initialise(configuration) == SP_ERROR_OK);
    });
    
    thread_pool.schedule([&]{
        session->Login(username.c_str(), password.c_str(), true);
    });
    
    boost::mutex next_timeout_mutex;
    int next_timeout = 0;
    boost::mutex notify_mutex;
    bool notify = false;
    boost::condition_variable condition;

    boost::function<void ()> reeschedule = [&] {
        boost::lock_guard<boost::mutex> lock(notify_mutex);
        {
            boost::lock_guard<boost::mutex> timeout_lock(next_timeout_mutex);
            next_timeout = session->Update();
        }
        notify = true;
        condition.notify_all();
    };

    session->connectToOnNotifyMainThread([&] {
        thread_pool.schedule(reeschedule);
    });

    bool done = false;
    boost::thread update_scheduler([&]{
        while (!done) {
            boost::unique_lock<boost::mutex> lock(notify_mutex);
            while (!notify) {
                if (!condition.timed_wait(lock, boost::posix_time::millisec(next_timeout))) {
                    // timeout, need to schedule update again
                    thread_pool.schedule(reeschedule);
                    // we use reeschedule, because we can reenter the while
                    // with the old timeout
                    notify = false;
                }
            }
            // update was called by someone else need to update timeout
            notify = false;
        }
    });

    while (!session->IsLoggedIn())
        boost::this_thread::sleep(boost::posix_time::millisec(20));

    done = true;
    update_scheduler.join();

    thread_pool.wait();
}

BOOST_AUTO_TEST_SUITE_END()
