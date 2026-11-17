#include <malloc.h>
#include <pthread.h>
#include <unistd.h>

struct job
{
  /* Link field for linked list.  */
  struct job *next;

  int job_id;

  int job_proccess_time

  /* Other fields describing work to be done... */
};

/* A linked list of pending jobs.  */
struct job *job_queue;

void process_job(struct job *j, int thread_id)
{
  printf("thread %d: start proccessing job: %d \n", thread_id, j->job_id);

  sleep(j->job_proccess_time);

  printf("finished job: %d \n", j->job_id);


}

/* A mutex protecting job_queue.  */
pthread_mutex_t job_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Process queued jobs until the queue is empty.  */

void *thread_function(void *arg)
{
  int thread_id = (int)arg;
  
  while (1)
  {
    struct job *next_job;

    /* Lock the mutex on the job queue.  */
    pthread_mutex_lock(&job_queue_mutex);
    /* Now it's safe to check if the queue is empty.  */
    if (job_queue == NULL)
      next_job = NULL;
    else
    {
      /* Get the next available job.  */
      next_job = job_queue;
      /* Remove this job from the list.  */
      job_queue = job_queue->next;
    }
    /* Unlock the mutex on the job queue, since we're done with the
       queue for now.  */
    pthread_mutex_unlock(&job_queue_mutex);

    /* Was tjobhe queue empty?  If so, end the thread.  */
    if (next_job == NULL)
      break;

    /* Carry out the work.  */
    process_job(next_job, thread_id);
    /* Clean up.  */
    free(next_job);
  }
  return NULL;
}

int main(int argv, char *argc[])
{
  int workers_count = 2;
  int jobs_count = 50;

  struct job *prev_job = malloc(sizeof(struct job));
  prev_job->job_id = 0;
  prev_job->job_proccess_time = 1;

  job_queue = prev_job;

  for (size_t i = 1; i < jobs_count; i++)
  {
    struct job *curr_job = malloc(sizeof(struct job));
    curr_job->job_id = i;
    curr_job->job_proccess_time = 1;
    prev_job->next = curr_job;
    prev_job = curr_job;
  }

  pthread_t *threads = malloc(sizeof(pthread_t) * workers_count);

  for (size_t i = 0; i < workers_count; i++)
  {
    int result = pthread_create(&threads[i], NULL, thread_function, i);

    if (result != 0)
    {
      perror("Failed to create thread");
      free(threads);
      return 1;
    }
  }

  for (size_t i = 0; i < workers_count; i++)
  {
    pthread_join(threads[i], NULL);
  }

  printf("all threads exited \n");
  return 0;
}
